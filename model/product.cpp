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
                _products.insert({obj->getValue<std::string>("name"),obj->getValue<std::string>("cluster")});
            }
        }
    }
    Poco::JSON::Array::Ptr Products::toJSON() const
    {
        Poco::JSON::Array::Ptr array = new Poco::JSON::Array();
        for (auto &p : _products)
        {
            Poco::JSON::Object::Ptr obj = new Poco::JSON::Object();
            obj->set("name",p.name);
            obj->set("cluster",p.cluster);
            array->add(obj);
        }

        return array;
    }

    Products &Products::get()
    {
        static Products instance;
        return instance;
    }

    const std::set<model::Product> &Products::products() const
    {
        return _products;
    }

    bool Product::operator<(const Product &other) const{
        return name<other.name;

    }
}