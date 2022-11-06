#include "product.h"

#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <fstream>


namespace model
{
    Products::Products()
    {
        std::ifstream ifs("products.json");
        if (ifs.is_open())
        {
            std::istream_iterator<char> start(ifs >> std::noskipws), end{};
            std::string str{start, end};
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result = parser.parse(str);
            Poco::JSON::Array::Ptr array = result.extract<Poco::JSON::Array::Ptr>();
            for (size_t i = 0; i < array->size(); ++i)
            {
                Poco::JSON::Object::Ptr obj = array->getObject(i);


                std::string product_name = obj->getValue<std::string>("name");
                std::string cluster_name = obj->getValue<std::string>("cluster");
                std::shared_ptr<Product> product = std::shared_ptr<Product>(new Product{product_name, cluster_name});


                _products[product_name] = product;
            }
        }
    }
    Poco::JSON::Array::Ptr Products::toJSON() const
    {
        Poco::JSON::Array::Ptr array = new Poco::JSON::Array();
        for (auto &[n, p] : _products)
            array->add(p->toJSON());

        return array;
    }

    Products &Products::get()
    {
        static Products instance;
        return instance;
    }

    std::map<std::string, std::shared_ptr<model::Product>> &Products::products()
    {
        return _products;
    }

    Poco::JSON::Object::Ptr Product::toJSON() const
    {
        Poco::JSON::Object::Ptr obj = new Poco::JSON::Object();
        obj->set("name", name);
        obj->set("cluster", cluster);

        return obj;
    }

    bool Product::operator<(const Product &other) const
    {
        return name < other.name;
    }
}