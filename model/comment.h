#ifndef COMMENTS_H
#define COMMENTS_H

#include <string>
#include "Poco/JSON/Object.h"

namespace model
{
    struct Comment{
        std::string product;
        std::string cluster_issue;
        std::string comment;
        std::string address;

        static Comment load(const std::string &product, const  std::string &cluster_issue);
        static Comment fromJSON(Poco::JSON::Object::Ptr ptr); 
        Poco::JSON::Object::Ptr toJSON();
        
        void save();
    };
} // namespace model



#endif