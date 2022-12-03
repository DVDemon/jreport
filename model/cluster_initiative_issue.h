#ifndef CLUSTER_INITIATIVE_ISSUE_H
#define CLUSTER_INITIATIVE_ISSUE_H

#include <set>
#include <string>
#include "Poco/JSON/Object.h"
#include <optional>

namespace model
{
    struct ClusterInitativeIssue{
        std::string cluster;
        std::string initiative;
        std::string initiative_issue;
        std::string issue;
        static std::optional<ClusterInitativeIssue> load(const std::string &cluster,const std::string &initiative_issue);
        static ClusterInitativeIssue fromJSON(Poco::JSON::Object::Ptr ptr); 
        void save();
    };
} // namespace model



#endif