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

            std::vector<IssueLink> _links;

        public:
            std::string& id();
            std::string& key();
            std::string& name();
            std::string& description();
            std::string& author();
            std::string& assignee();
            std::string& status();
            std::vector<IssueLink>& links();

            const std::string& get_id();
            const std::string& get_key();
            const std::string& get_name();
            const std::string& get_description();
            const std::string& get_author();
            const std::string& get_assignee();
            const std::string& get_status();
            const std::vector<IssueLink>& get_links();

            static void init();
            static Issue read_by_id(const std::string& id);
            static std::vector<Issue> read_all();
            void save_to_mysql();

            static Issue fromJSON(const std::string & str);
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