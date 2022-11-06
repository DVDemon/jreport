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

#include <memory>
#include <fstream>
#include <functional>

namespace loaders
{
    LoaderJira::LoaderJira()
    {
        Poco::Net::initializeSSL();
        Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> ptrCert = new Poco::Net::AcceptCertificateHandler(false);
        Poco::Net::Context::Ptr ptrContext = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", "rootcert.pem", Poco::Net::Context::VERIFY_STRICT, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
        Poco::Net::SSLManager::instance().initializeClient(0, ptrCert, ptrContext);
    }

    LoaderJira &LoaderJira::get()
    {
        static LoaderJira _instance;
        return _instance;
    }

    std::string LoaderJira::load_from_file(const std::string &id)
    {
        
        std::string file_name;
        size_t index = id.find_last_of('/');
        if (index == std::string::npos) file_name = "stubs/" + id + ".json";
         else file_name = "stubs/" + id.substr(index + 1) + ".json";
        
            
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

   std::shared_ptr<model::Issue> LoaderJira::load([[maybe_unused]] const std::string &id)
    {
#ifdef STUB
        std::string string_result = load_from_file(id);
#elif
        try
        {
            std::string request_uri {"http://jira.mts.ru/rest/api/2/issue/"};
            request_uri+=id;
            Poco::URI uri("https://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&hourly=temperature_2m,relativehumidity_2m,windspeed_10m");
            Poco::Net::HTTPSClientSession s(uri.getHost(), uri.getPort());
            Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri.toString());
            request.set("user-agent", "Poco HTTPSClientSession");
            s.sendRequest(request);
            Poco::Net::HTTPResponse response;
            std::istream &rs = s.receiveResponse(response);
            Poco::StreamCopier::copyStream(rs, std::cout);
        }
        catch (Poco::Exception &ex)
        {
            std::cout << ex.displayText() << std::endl;
        }
#endif
        if (!string_result.empty())
        {
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result = parser.parse(string_result);
            Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

            std::function<std::shared_ptr<model::Issue>(Poco::JSON::Object::Ptr)> create = [&](Poco::JSON::Object::Ptr object)
            {
                std::shared_ptr<model::Issue> issue = std::shared_ptr<model::Issue>(new model::Issue());
                issue->id() =  object->getValue<std::string>("id");            
                issue->key() =  object->getValue<std::string>("key");

                if(object->has("fields")){
                    object = object->getObject("fields");

                    if(object->has("resolution")){
                            Poco::JSON::Object::Ptr res = object->getObject("resolution");
                            if(res)
                            if(res->has("name"))
                            issue->resolution() = res->getValue<std::string>("name");
                    }

                    if(object->has("description"))
                    issue->description() =  object->getValue<std::string>("description");
                    if(object->has("summary"))
                    issue->name() = object->getValue<std::string>("summary");

                    if(object->has("status"))
                    issue->status() = object->getObject("status")->getValue<std::string>("name");
                    if(object->has("creator"))
                    issue->author() = object->getObject("creator")->getValue<std::string>("displayName");
                    if(object->has("assignee"))
                    issue->assignee() = object->getObject("assignee")->getValue<std::string>("displayName");

                    if(object->has("issuelinks")){
                        Poco::JSON::Array::Ptr links = object->getArray("issuelinks");
                        if(links)
                        for(size_t i=0;i<links->size();++i){
                            Poco::JSON::Object::Ptr link = links->getObject(i);
                            std::string type;
                            if(link->has("outwardIssue")){
                                    type = link->getObject("type")->getValue<std::string>("outward");
                                    std::shared_ptr<model::Issue> linked_element = create(link->getObject("outwardIssue"));
                                    issue->links().push_back(model::IssueLink(type,linked_element));
                            } else
                            if(link->has("inwardIssue")){
                                    type = link->getObject("type")->getValue<std::string>("inward");
                                    std::shared_ptr<model::Issue> linked_element = create(link->getObject("inwardIssue"));
                                    issue->links().push_back(model::IssueLink(type,linked_element));
                            } 

                        }

                        std::sort(std::begin(issue->links()),std::end(issue->links()),[](const model::IssueLink &lhv,const model::IssueLink &rhv){
                            return lhv.item->get_name()<rhv.item->get_name();
                        });
                    }
                }

               return issue; 
            };

            return create(object);
        }

        return std::shared_ptr<model::Issue>();
    }

    LoaderJira::~LoaderJira()
    {
        Poco::Net::uninitializeSSL();
    }
}