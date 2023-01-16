#include "issue.h"
#include "../database/database.h"
#include "../database/cache.h"

#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

#include <Poco/MongoDB/Cursor.h>

namespace model
{
    std::string &Issue::id()
    {
        return _id;
    }

    std::string &Issue::key()
    {
        return _key;
    }

    std::string &Issue::name()
    {
        return _name;
    }
    std::string &Issue::description()
    {
        return _description;
    }
    std::string &Issue::author()
    {
        return _author;
    }
    std::string &Issue::assignee()
    {
        return _assignee;
    }
    std::string &Issue::status()
    {
        return _status;
    }

    std::string &Issue::resolution()
    {
        return _resolution;
    }
    std::string &Issue::project()
    {
        return _project;
    }

    std::string &Issue::product()
    {
        return _product;
    }

    const std::string &Issue::get_product()
    {
        return _product;
    }

    const std::string &Issue::get_project()
    {
        return _project;
    }

    const std::string &Issue::get_id()
    {
        return _id;
    }

    const std::string &Issue::get_key()
    {
        return _key;
    }
    const std::string &Issue::get_name()
    {
        return _name;
    }
    const std::string &Issue::get_description()
    {
        return _description;
    }
    const std::string &Issue::get_author()
    {
        return _author;
    }
    const std::string &Issue::get_assignee()
    {
        return _assignee;
    }
    const std::string &Issue::get_status()
    {
        return _status;
    }

    const std::string &Issue::get_resolution()
    {
        return _resolution;
    }

    std::vector<std::string>& Issue::hrefs(){
        return _hrefs;
    }
    
    const std::vector<std::string>& Issue::get_hrefs(){
        return _hrefs;
    }

    void Issue::save_to_cache(){
        std::stringstream ss;
        Poco::JSON::Stringifier::stringify(toJSON(), ss);
        std::string message = ss.str();
        database::Cache::get().put(get_key(), message);
    }

    std::shared_ptr<model::Issue> Issue::from_cache(const std::string & key){
         try
        {
            std::string result;
            if (database::Cache::get().get(key, result))
                return fromJSON(result);
            //else
                
        }
        catch (std::exception* err)
        {
            //std::cerr << "error:" << err->what() << std::endl;
            //throw;
        }
        return std::shared_ptr<Issue>();
    }

// https://github.com/pocoproject/poco/blob/devel/MongoDB/samples/SQLToMongo/src/SQLToMongo.cpp
    void Issue::save_to_mongodb()
    {
        Poco::MongoDB::Database db("jreport");
        try
        {
            const std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
            const std::time_t t_t = std::chrono::system_clock::to_time_t(now);
            tm local_tm = *localtime(&t_t);
            std::string date_key;
            date_key += std::to_string(local_tm.tm_year + 1900) + 
                ((local_tm.tm_mon + 1<10)?"0":"") + std::to_string(local_tm.tm_mon + 1) + 
                ((local_tm.tm_mday <10)?"0":"") + std::to_string(local_tm.tm_mday);

            Poco::SharedPtr<Poco::MongoDB::DeleteRequest> request = db.createDeleteRequest("issues");
            request->selector().add("key", get_key());
            request->selector().add("date", date_key);
            database::Database::get().mongo().sendRequest(*request);

            Poco::SharedPtr<Poco::MongoDB::InsertRequest> insertRequest = db.createInsertRequest("issues");

            std::stringstream ss;
            Poco::JSON::Stringifier::stringify(toJSON(), ss, 4, -1, Poco::JSON_PRESERVE_KEY_ORDER);

            insertRequest->addNewDocument()
                .add("key", get_key())
                .add("date", date_key)
                .add("document", ss.str());

            database::Database::get().mongo().sendRequest(*insertRequest);
            std::string lastError = db.getLastError(database::Database::get().mongo());
            if (!lastError.empty())
            {
                std::cout << "mongodb Last Error: " << db.getLastError(database::Database::get().mongo()) << std::endl;
            }
           // else
           //     std::cout << "mongodb: Saved" << std::endl;
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

    std::shared_ptr<model::Issue> Issue::from_mongodb(const std::string &key, const tm &date)
    {
        std::shared_ptr<model::Issue> result;

        try
        {
            std::string date_key;
            date_key += std::to_string(date.tm_year + 1900) + 
                ((date.tm_mon + 1<10)?"0":"") + std::to_string(date.tm_mon + 1) + 
                ((date.tm_mday <10)?"0":"") + std::to_string(date.tm_mday);

            Poco::MongoDB::Cursor cursor("jreport", "issues");
            cursor.query().selector().add("key", key);
            cursor.query().selector().add("date", date_key);

            //std::cout << "mongodb: query key='" + key + "' date='" + date_key + "'" << std::endl;

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

    std::shared_ptr<Issue> Issue::fromJSON(const std::string &str)
    {
        // std::cout << str << std::endl << std::endl;
        std::shared_ptr<Issue> issue = std::make_shared<Issue>();
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        issue->id() = object->getValue<std::string>("id");
        issue->key() = object->getValue<std::string>("key");
        issue->name() = object->getValue<std::string>("name");
        issue->description() = object->getValue<std::string>("description");
        issue->author() = object->getValue<std::string>("author");
        issue->assignee() = object->getValue<std::string>("assignee");
        issue->status() = object->getValue<std::string>("status");
        issue->resolution() = object->getValue<std::string>("resolution");
        issue->project() = object->getValue<std::string>("project");
        issue->product() = object->getValue<std::string>("product");

        if(object->has("links")){
            Poco::JSON::Array::Ptr links = object->getArray("links");
            if (links)
            {
                for (size_t i = 0; i < links->size(); ++i)
                {
                    Poco::JSON::Object::Ptr l = links->getObject(i);
                    IssueLink il;
                    il.link_type = l->getValue<std::string>("type");
                    Poco::JSON::Object::Ptr item = l->getObject("issue");

                    std::stringstream ss;
                    Poco::JSON::Stringifier::stringify(item, ss, 4, -1, Poco::JSON_PRESERVE_KEY_ORDER);
                    il.item = Issue::fromJSON(ss.str());
                    issue->links().push_back(il);
                }
            }
        }

        if(object->has("hrefs")){
            Poco::JSON::Array::Ptr hrefs = object->getArray("hrefs");
            if (hrefs)
            {
                for (size_t i = 0; i < hrefs->size(); ++i)
                {
                    std::string val = hrefs->getElement<std::string>(i);
                    issue->hrefs().push_back(val);
                }
            }
        }

        return issue;
    }

    Poco::JSON::Object::Ptr Issue::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("id", _id);
        root->set("key", _key);
        root->set("name", _name);
        root->set("description", _description);
        root->set("author", _author);
        root->set("assignee", _assignee);
        root->set("status", _status);
        root->set("resolution", _resolution);
        root->set("project", _project);
        root->set("product", _product);

        if (!_hrefs.empty()){
            Poco::JSON::Array::Ptr hrefs_array = new Poco::JSON::Array();
            for(const std::string & s: _hrefs){
                hrefs_array->add(s);
            }
        }
        if (!_links.empty())
        {
            Poco::JSON::Array::Ptr links_array = new Poco::JSON::Array();
            for (size_t i = 0; i < _links.size(); ++i)
            {
                Poco::JSON::Object::Ptr l = new Poco::JSON::Object();
                l->set("type", _links[i].link_type);
                l->set("issue", _links[i].item->toJSON());
                links_array->add(l);
            }
            root->set("links", links_array);
        }

        
        return root;
    }

    std::vector<IssueLink> &Issue::links()
    {
        return _links;
    }

    const std::vector<IssueLink> &Issue::get_links()
    {
        return _links;
    }

    IssueLink::IssueLink() {}
    IssueLink::IssueLink(std::string lt, std::shared_ptr<Issue> i) : link_type(lt), item(i)
    {
    }
}

std::ostream &operator<<(std::ostream &os, model::Issue &issue)
{
    os << "id:" << issue.get_id() << std::endl;
    os << "key:" << issue.get_key() << std::endl;
    os << "name:" << issue.get_name() << std::endl;
    os << "description:" << issue.get_description() << std::endl;
    os << "author:" << issue.get_author() << std::endl;
    os << "get_assignee:" << issue.get_assignee() << std::endl;
    os << "status:" << issue.get_status() << std::endl;
    os << "project:" << issue.get_project() << std::endl;
    os << "product:" << issue.get_product() << std::endl;

    return os;
}
