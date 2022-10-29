#ifndef CLUSTER_H
#define CLUSTER_H

#include <set>
#include <string>
#include "Poco/JSON/Object.h"

namespace model
{
    class Cluster{
        private:
            std::set<std::string> _clusters;
            Cluster();
        public:
            static Cluster& get();
            const std::set<std::string> &clusters() const;
            Poco::JSON::Array::Ptr toJSON() const;
    };
} // namespace model



#endif