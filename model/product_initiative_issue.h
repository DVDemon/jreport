#ifndef PRODUCT_INITIATIVE_ISSUE_H
#define PRODUCT_INITIATIVE_ISSUE_H

#include <set>
#include <string>
#include <optional>
#include "Poco/JSON/Object.h"

namespace model
{
    struct ProductInitativeIssue{
        std::string product;
        std::string cluster_issue;
        std::string product_issue;

        static ProductInitativeIssue load(const std::string &product, const  std::string &cluster_issue);
        static std::optional<ProductInitativeIssue> load_by_issue(std::string &product_issue, std::string &cluster_issue);
        static std::vector<ProductInitativeIssue> load_by_cluster_issue(std::string &cluster_issue);
        static std::vector<ProductInitativeIssue> load_all();
        static ProductInitativeIssue fromJSON(Poco::JSON::Object::Ptr ptr); 
        void save();
    };
} // namespace model



#endif