#pragma once

#include <string>
#include <optional>
#include "Poco/JSON/Object.h"

namespace model
{
    struct ClusterProject{
        std::string cluster;
        std::string project;


        static std::optional<ClusterProject> load(const std::string &project);
        static ClusterProject fromJSON(Poco::JSON::Object::Ptr ptr);
        static void fill(); 
        Poco::JSON::Object::Ptr toJSON();
        
        void save();
    };
} // namespace model

