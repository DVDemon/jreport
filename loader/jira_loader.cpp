#include "jira_loader.h"

#include "../config/config.h"
#include "../database/database.h"
#include "url_downloader.h"

#include <Poco/MongoDB/Cursor.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Foundation.h>
#include <Poco/RegularExpression.h>

#include <memory>
#include <fstream>

namespace loaders
{
    LoaderJira::LoaderJira()
    {
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

                    if (object->has("comment")){
                        Poco::JSON::Object::Ptr comment = object->getObject("comment");
                        //std::cout << "++++++++ comment" << std::endl;
                        if(comment->has("comments")){
                            //std::cout << "++++++++ comments" << std::endl;
                            Poco::JSON::Array::Ptr comments = comment->getArray("comments");
                            if (comments){
                                    //std::cout << "++++++++ comments:" << comments->size()  << std::endl;
                                    
                                    for (size_t i = 0; i < comments->size(); ++i)
                                    {
                                        //std::cout << "+++++++++ c:" << i << std::endl;
                                        Poco::JSON::Object::Ptr com = comments->getObject(i);
                                        std::string body;
                                        if(com->has("body")){
                                            body = com->getValue<std::string>("body");
                                            //std::cout << "++++++++++++++++++++++++++[" << body <<"]" <<std::endl;
                                            
                                            std::string href;
                                            Poco::RegularExpression regex("(http|ftp|https):\\/\\/([\\w_-]+(?:(?:\\.[\\w_-]+)+))([\\w.,@?^=%&:\\/~+#-]*[\\w@?^=%&\\/~+#-])");
                                            Poco::RegularExpression::MatchVec mvec;
                                            int matches = regex.extract(body,href);
                                            if(matches>0){
                                                issue->hrefs().push_back(href);
                                                std::cout << "--------------------------[" << href <<"]" <<std::endl;
                                            }
                                        }
                                    }
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

    std::shared_ptr<model::Issue> LoaderJira::load( const std::string &id, [[maybe_unused]] const std::string &identity)
    {
        std::shared_ptr<model::Issue> res;
       // res = model::Issue::from_cache(id);
        if(res) {
            //std::cout << "loaded from cache" << std::endl;
            return res;
        }
#ifdef STUB
        std::string string_result = load_from_file(id);
#endif

#ifndef STUB
        std::string string_result;

        std::string request_uri{Config::get().jira_address()};
        request_uri += "/rest/api/2/issue/"+ id;
        std::optional<std::string> ptr = loaders::Downloader::get().do_get(request_uri,identity);
        if(ptr) string_result = *ptr;
            else throw std::logic_error("download exception");
#endif
        
        if(!string_result.empty()){
            res = parse(string_result);
            if(res) res->save_to_cache();
        }

        return res;
    }

    LoaderJira::~LoaderJira()
    {
        
    }
}