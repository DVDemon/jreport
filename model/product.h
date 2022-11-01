#ifndef PRODUCT_H
#define PRODUCT_H

#include <map>
#include <string>
#include <memory>
#include "Poco/JSON/Object.h"

#include "initiative.h"

namespace model
{
    struct Product{
        std::string name;
        std::string cluster;
        std::map<std::string,Initiative> issues;
        Poco::JSON::Object::Ptr toJSON() const;
        bool operator<(const Product &other) const;
    };

    class Products{
        private:
            std::map<std::string,std::shared_ptr<Product>> _products;
            Products();
        public:
            static Products& get();
            std::map<std::string,std::shared_ptr<Product>> &products();
            Poco::JSON::Array::Ptr toJSON() const;
    };
} // namespace model

#endif