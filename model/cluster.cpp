#include "cluster.h"

#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>
#include <fstream>

namespace model
{
    Cluster::Cluster()
    {
        std::ifstream ifs("clusters.json");
        if (ifs.is_open())
        {
            std::istream_iterator<char> start(ifs >> std::noskipws), end{};
            std::string str{start, end};
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result = parser.parse(str);
            Poco::JSON::Array::Ptr array = result.extract<Poco::JSON::Array::Ptr>();
            for(size_t i=0;i<array->size();++i){
                std::string item = array->getElement<std::string>(i);
                _clusters.insert(item);
            }
        }
    }

    Poco::JSON::Array::Ptr Cluster::toJSON() const{
        Poco::JSON::Array::Ptr array = new Poco::JSON::Array();
        for(auto cluster : _clusters){
            array->add(cluster.c_str());
        }
            
        return array;
    }

    Cluster &Cluster::get()
    {
        static Cluster instance;
        return instance;
    }

    const std::set<std::string> &Cluster::clusters() const
    {
        return _clusters;
    }
} // namespace model
