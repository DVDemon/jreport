#include "jira_loader.h"

#include "Poco/StreamCopier.h"
#include "Poco/URI.h"
#include "Poco/Exception.h"
#include "Poco/SharedPtr.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/Net/KeyConsoleHandler.h"
#include "Poco/Net/ConsoleCertificateHandler.h"
#include "Poco/Net/AcceptCertificateHandler.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <Poco/URIStreamFactory.h>
#include <Poco/URIStreamOpener.h>
#include <Poco/Net/HTTPSStreamFactory.h>
#include <Poco/Net/HTTPStreamFactory.h>
#include <Poco/Base64Encoder.h>

#include "../config/config.h"
#include "../database/database.h"

#include <Poco/MongoDB/Cursor.h>

#include <memory>
#include <fstream>
#include <functional>

namespace loaders
{
    LoaderJira::LoaderJira()
    {

        Poco::Net::initializeSSL();
        Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> ptrCert = new Poco::Net::AcceptCertificateHandler(false);
        Poco::Net::Context::Ptr ptrContext = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "");
        Poco::Net::SSLManager::instance().initializeClient(0, ptrCert, ptrContext);
        std::cout << "inited ..." << std::endl;
    }

    LoaderJira &LoaderJira::get()
    {
        static LoaderJira _instance;
        return _instance;
    }

// https://github.com/pocoproject/poco/blob/devel/MongoDB/samples/SQLToMongo/src/SQLToMongo.cpp

    void LoaderJira::save([[maybe_unused]] std::shared_ptr<model::Issue> issue)
    {
        if (issue)
        {
            Poco::MongoDB::Database db("jreport");
            try
            {
                const std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
                const std::time_t t_t = std::chrono::system_clock::to_time_t(now);
                tm local_tm = *localtime(&t_t);
                std::string date_key;
                date_key += std::to_string(local_tm.tm_year + 1900) + std::to_string(local_tm.tm_mon + 1) + std::to_string(local_tm.tm_mday);

                Poco::SharedPtr<Poco::MongoDB::DeleteRequest> request = db.createDeleteRequest("issues");
                request->selector().add("key", issue->get_key());
                request->selector().add("date", date_key);
                database::Database::get().mongo().sendRequest(*request);

                Poco::SharedPtr<Poco::MongoDB::InsertRequest> insertRequest = db.createInsertRequest("issues");

                std::stringstream ss;
                Poco::JSON::Stringifier::stringify(issue->toJSON(), ss, 4, -1, Poco::JSON_PRESERVE_KEY_ORDER);

                insertRequest->addNewDocument()
                    .add("key", issue->get_key())
                    .add("date", date_key)
                    .add("document", ss.str());

                database::Database::get().mongo().sendRequest(*insertRequest);
                std::string lastError = db.getLastError(database::Database::get().mongo());
                if (!lastError.empty())
                {
                    std::cout << "mongodb Last Error: " << db.getLastError(database::Database::get().mongo()) << std::endl;
                }
                else
                    std::cout << "mongodb: Saved" << std::endl;
            }
            catch (std::exception &ex)
            {
                std::cout << "mongodb exception: " << ex.what() << std::endl;
                std::string lastError = db.getLastError(database::Database::get().mongo());
                if (!lastError.empty())
                {
                    std::cout << "mongodb Last Error: " << db.getLastError(database::Database::get().mongo()) << std::endl;
                }
            }
        }
    }

    std::shared_ptr<model::Issue> LoaderJira::load_by_date(const std::string &key, const tm &date)
    {
        std::shared_ptr<model::Issue> result;

        try
        {
            std::string date_key;
            date_key += std::to_string(date.tm_year + 1900) + std::to_string(date.tm_mon + 1) + std::to_string(date.tm_mday);

            Poco::MongoDB::Cursor cursor("jreport", "issues");
            cursor.query().selector().add("key", key);
            cursor.query().selector().add("date", date_key);

            std::cout << "mongodb: query key='" + key + "' date='" + date_key + "'" << std::endl;

            Poco::MongoDB::ResponseMessage &response = cursor.next(database::Database::get().mongo());
            for (;;)
            {
                for (Poco::MongoDB::Document::Vector::const_iterator it = response.documents().begin(); it != response.documents().end(); ++it)
                {
                    result = model::Issue::fromJSON((*it)->get<std::string>("document"));
                    auto pp = result->toJSON();
                    std::stringstream ss;
                    Poco::JSON::Stringifier::stringify(pp, ss, 4, -1, Poco::JSON_PRESERVE_KEY_ORDER); 

                    return result;
                }

                // When the cursorID is 0, there are no documents left, so break out ...
                if (response.cursorID() == 0)
                {
                    break;
                }

                // Get the next bunch of documents
                response = cursor.next(database::Database::get().mongo());
            }
        }
        catch (std::exception &ex)
        {
            std::cout << "mongodb exception: " << ex.what() << std::endl;
        }

        return result;
    }

    std::string LoaderJira::load_from_file(const std::string &id)
    {

        std::string file_name;
        size_t index = id.find_last_of('/');
        if (index == std::string::npos)
            file_name = "stubs/" + id + ".json";
        else
            file_name = "stubs/" + id.substr(index + 1) + ".json";

        std::ifstream ifs(file_name);
        std::cout << "loading from " << file_name << std::endl;

        if (ifs.is_open())
        {
            std::istream_iterator<char> start{ifs >> std::noskipws}, end{};
            std::string result{start, end};

            return result;
        }

        return std::string();
    }

    std::shared_ptr<model::Issue> LoaderJira::parse(const std::string &source)
    {
        if (!source.empty())
        {
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result = parser.parse(source);
            Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

            std::function<std::shared_ptr<model::Issue>(Poco::JSON::Object::Ptr)> create = [&](Poco::JSON::Object::Ptr object)
            {
                std::shared_ptr<model::Issue> issue = std::shared_ptr<model::Issue>(new model::Issue());
                issue->id() = object->getValue<std::string>("id");
                issue->key() = object->getValue<std::string>("key");

                if (object->has("fields"))
                {
                    object = object->getObject("fields");

                    if (object->has("resolution"))
                    {
                        Poco::JSON::Object::Ptr res = object->getObject("resolution");
                        if (res)
                            if (res->has("name"))
                                issue->resolution() = res->getValue<std::string>("name");
                    }

                    if (object->has("description"))
                        issue->description() = object->getValue<std::string>("description");

                    if (object->has("summary"))
                        issue->name() = object->getValue<std::string>("summary");

                    if (object->has("status"))
                        if (object->getObject("status")->has("name"))
                            issue->status() = object->getObject("status")->getValue<std::string>("name");

                    if (object->has("creator"))
                        if (object->getObject("creator")->has("displayName"))
                            issue->author() = object->getObject("creator")->getValue<std::string>("displayName");

                    if (object->has("assignee"))
                        if (object->getObject("assignee")->has("displayName"))
                            issue->assignee() = object->getObject("assignee")->getValue<std::string>("displayName");

                    if (object->has("project"))
                        if (object->getObject("project")->has("key"))
                            issue->project() = object->getObject("project")->getValue<std::string>("key");

                    if (object->has("components"))
                    {
                        Poco::JSON::Array::Ptr comps = object->getArray("components");
                        if (comps)
                            for (size_t i = 0; i < comps->size(); ++i)
                            {
                                Poco::JSON::Object::Ptr comp = comps->getObject(i);
                                if (comp->has("description"))
                                {
                                    std::string type = comp->getValue<std::string>("description");
                                    if (type == "Product")
                                        if (comp->has("name"))
                                            issue->product() = comp->getValue<std::string>("name");
                                }
                            }
                    }

                    if (object->has("issuelinks"))
                    {
                        Poco::JSON::Array::Ptr links = object->getArray("issuelinks");
                        if (links)
                            for (size_t i = 0; i < links->size(); ++i)
                            {
                                Poco::JSON::Object::Ptr link = links->getObject(i);
                                std::string type;

                                if (link->has("outwardIssue") && link->has("type"))
                                {
                                    type = link->getObject("type")->getValue<std::string>("outward");
                                    std::shared_ptr<model::Issue> linked_element = create(link->getObject("outwardIssue"));
                                    issue->links().push_back(model::IssueLink(type, linked_element));
                                }
                                else if (link->has("inwardIssue") && link->has("type"))
                                {
                                    type = link->getObject("type")->getValue<std::string>("inward");
                                    std::shared_ptr<model::Issue> linked_element = create(link->getObject("inwardIssue"));
                                    issue->links().push_back(model::IssueLink(type, linked_element));
                                }
                            }

                        std::sort(std::begin(issue->links()), std::end(issue->links()), [](const model::IssueLink &lhv, const model::IssueLink &rhv)
                                  { return lhv.item->get_name() < rhv.item->get_name(); });
                    }
                }

                return issue;
            };

            return create(object);
        }

        return std::shared_ptr<model::Issue>();
    }

    std::shared_ptr<model::Issue> LoaderJira::load([[maybe_unused]] const std::string &id, const std::string &identity)
    {
#ifdef STUB
        std::string string_result = load_from_file(id);
#endif

#ifndef STUB
        std::string string_result;
        try
        {

            std::string request_uri{Config::get().jira_address()};
            std::string resource{"/rest/api/2/issue/"};
            resource += id;
            // std::cout << resource << std::endl;
            Poco::URI uri(request_uri + resource);
            Poco::Net::HTTPSClientSession s(uri.getHost(), uri.getPort());
            Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri.toString());
            request.setContentType("application/json");

            // std::cout << identity << std::endl;
            request.set("Authorization", identity);
            request.setKeepAlive(true);

            s.sendRequest(request);
            Poco::Net::HTTPResponse response;
            std::istream &rs = s.receiveResponse(response);

            while (rs)
            {
                char c{};
                rs.read(&c, 1);
                if (rs)
                    string_result += c;
            }
        }
        catch (Poco::Exception &ex)
        {
            std::cout << ex.displayText() << std::endl;
            throw;
        }
#endif
        

        return parse(string_result);
    }

    LoaderJira::~LoaderJira()
    {
        Poco::Net::uninitializeSSL();
    }
}