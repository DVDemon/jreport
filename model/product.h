#ifndef PRODUCT_H
#define PRODUCT_H

#include <set>
#include <string>
#include "Poco/JSON/Object.h"

#include "initiative.h"

namespace model
{
    struct Product{
        std::string name;
        std::string cluster;
        std::map<std::string,Initiative> issues;
        bool operator<(const Product &other) const;
    };

    class Products{
        private:
            std::set<Product> _products;
            Products();
        public:
            static Products& get();
            const std::set<Product> &products() const;
            Poco::JSON::Array::Ptr toJSON() const;
    };
} // namespace model

#endif