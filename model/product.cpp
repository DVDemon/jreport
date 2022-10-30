#include "product.h"

#include "../database/database.h"

#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <fstream>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

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


                std::set<std::string> issues;
                Poco::Data::Session session = database::Database::get().create_session();            
                Statement select(session);
                std::string issue_name;
                std::string issue_key;
                std::string product_name = obj->getValue<std::string>("name");
                std::string cluster_name = obj->getValue<std::string>("cluster");
                Product product{product_name,cluster_name};

                select << "SELECT initative_name,issue_key FROM Product_Issue WHERE product_name=?",
                    into(issue_name),
                    into(issue_key),
                    use(product_name),
                    range(0, 1); //  iterate over result set one row at a time

                while (!select.done())
                {
                    if(select.execute()){
                        if(product.issues.find(issue_name)!=std::end(product.issues))
                            product.issues[issue_name] = Initiative({issue_name});
                         product.issues[issue_name].name = issue_name;
                         product.issues[issue_name].issues.insert(issue_key);
                    }                    
                }


                _products.insert(product);
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
            Poco::JSON::Array::Ptr issues = new Poco::JSON::Array();
            for(const auto& [in_name,sn]: p.issues){
                Poco::JSON::Object::Ptr i = new Poco::JSON::Object();
                i->set("name",sn.name);
                Poco::JSON::Array::Ptr initiatives = new Poco::JSON::Array();
                for(const std::string &s:sn.issues){
                    initiatives->add(s.c_str());
                }
                i->set("issues",initiatives);
                issues->add(i);
            }
            obj->set("initiatives",issues);
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