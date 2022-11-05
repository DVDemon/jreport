#ifndef PRODUCT_INITIATIVE_ISSUE_H
#define PRODUCT_INITIATIVE_ISSUE_H

#include <set>
#include <string>
#include "Poco/JSON/Object.h"

namespace model
{
    struct ProductInitativeIssue{
        std::string product;
        std::string cluster_issue;
        std::string product_issue;

        static ProductInitativeIssue load(std::string &product, std::string &cluster_issue);
        static ProductInitativeIssue load_by_issue(std::string &product_issue, std::string &cluster_issue);
        static ProductInitativeIssue fromJSON(Poco::JSON::Object::Ptr ptr); 
        void save();
    };
} // namespace model



#endif