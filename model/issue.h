#ifndef ISSUE_H
#define ISSUE_H

#include <string>
#include <vector>
#include <ostream>
#include <memory>
#include "Poco/JSON/Object.h"

namespace model
{
    struct IssueLink;

    class Issue{
        private:
            std::string _id;
            std::string _key;
            std::string _name;
            std::string _description;
            std::string _author;
            std::string _assignee;
            std::string _status;
            std::string _resolution;
            std::string _project;
            std::string _product;
            std::string _reason;

            std::vector<IssueLink> _links;
            std::vector<std::string> _hrefs;

        public:
            std::string& id();
            std::string& key();
            std::string& name();
            std::string& description();
            std::string& author();
            std::string& assignee();
            std::string& status();
            std::string& resolution();
            std::string& project();
            std::string& product();
            std::string& reason();
            std::vector<IssueLink>& links();
            std::vector<std::string>& hrefs();

            const std::string& get_id() const;
            const std::string& get_key() const;
            const std::string& get_name() const;
            const std::string& get_description() const;
            const std::string& get_author() const;
            const std::string& get_assignee() const;
            const std::string& get_status() const;
            const std::string& get_resolution() const;
            const std::string& get_project() const;
            const std::string& get_product() const;
            const std::string& get_reason() const;
            const std::vector<IssueLink>& get_links() const;
            const std::vector<std::string>& get_hrefs() const;
            
            void save_to_mongodb();
            void save_to_cache();
            static std::shared_ptr<Issue> from_cache(const std::string & key);
            static std::shared_ptr<Issue> from_mongodb(const std::string & key,const tm &date);
            static std::shared_ptr<Issue> fromJSON(const std::string & str);
            Poco::JSON::Object::Ptr toJSON() const;
    };

    struct IssueLink{
        std::string link_type;
        std::shared_ptr<Issue>   item;

        IssueLink();
        IssueLink(std::string link_type,std::shared_ptr<Issue>   item);
    };


}

std::ostream & operator<<(std::ostream&,model::Issue&); 

#endif