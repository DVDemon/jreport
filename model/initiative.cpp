#include "initiative.h"

#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>
#include <fstream>

namespace model
{
    Initiative::Initiative()
    {
        std::ifstream ifs("initiatives.json");
        if (ifs.is_open())
        {
            std::istream_iterator<char> start(ifs >> std::noskipws), end{};
            std::string str{start, end};
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result = parser.parse(str);
            Poco::JSON::Array::Ptr array = result.extract<Poco::JSON::Array::Ptr>();
            for(size_t i=0;i<array->size();++i){
                std::string item = array->getElement<std::string>(i);
                _initiatives.insert(item);
            }
        }
    }

    Initiative &Initiative::get()
    {
        static Initiative instance;
        return instance;
    }

    const std::set<std::string> &Initiative::initiatives() const
    {
        return _initiatives;
    }
} // namespace model