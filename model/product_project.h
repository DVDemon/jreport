#ifndef PRODUCT_PROJECT_H
#define PRODUCT_PROJECT_H

#include <string>
#include <optional>
#include "Poco/JSON/Object.h"

namespace model
{
    struct ProductProject{
        std::string product;
        std::string project;


        static std::optional<ProductProject> load(const std::string &product);
        static ProductProject fromJSON(Poco::JSON::Object::Ptr ptr); 
        Poco::JSON::Object::Ptr toJSON();
        
        void save();
    };
} // namespace model



#endif