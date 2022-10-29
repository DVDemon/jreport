#ifndef CLUSTER_H
#define CLUSTER_H

#include <set>
#include <string>

namespace model
{
    class Cluster{
        private:
            std::set<std::string> _clusters;
            Cluster();
        public:
            static Cluster& get();
            const std::set<std::string> &clusters() const;
    };
} // namespace model



#endif