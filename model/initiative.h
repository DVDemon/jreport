#ifndef INITIATIVE_H
#define INITIATIVE_H

#include <set>
#include <string>
#include <memory>

#include "Poco/JSON/Object.h"

namespace model
{
    struct Initiative{
        std::string name;
        std::set<std::string> issues;
        bool operator<(const Initiative &other) const;
    };

    class Initiatives{
        private:
            std::set<std::shared_ptr<Initiative>> _initiatives;
            Initiatives();
        public:
            static Initiatives& get();
            std::set<std::shared_ptr<Initiative>> &initiatives();
            Poco::JSON::Array::Ptr toJSON() const;
    };
} // namespace model

#endif