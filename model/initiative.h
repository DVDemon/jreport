#ifndef INITIATIVE_H
#define INITIATIVE_H

#include <set>
#include <string>
#include "Poco/JSON/Object.h"

namespace model
{
    class Initiative{
        private:
            std::set<std::string> _initiatives;
            Initiative();
        public:
            static Initiative& get();
            const std::set<std::string> &initiatives() const;
            Poco::JSON::Array::Ptr toJSON() const;
    };
} // namespace model

#endif