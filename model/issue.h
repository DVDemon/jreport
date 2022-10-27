#ifndef ISSUE_H
#define ISSUE_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"

namespace model
{
    class Issue{
        private:
            std::string _id;
            std::string _name;
            std::string _description;
            std::string _author;
            std::string _assignee;
            std::string _status;

        public:
            std::string& id();
            std::string& name();
            std::string& description();
            std::string& author();
            std::string& assignee();
            std::string& status();

            const std::string& get_id();
            const std::string& get_name();
            const std::string& get_description();
            const std::string& get_author();
            const std::string& get_assignee();
            const std::string& get_status();

            static void init();
            static Issue read_by_id(std::string& id);
            static std::vector<Issue> read_all();
            void save_to_mysql();

            static Issue fromJSON(const std::string & str);
            Poco::JSON::Object::Ptr toJSON() const;

    };
}

#endif