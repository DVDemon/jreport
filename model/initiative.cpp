#include "initiative.h"

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
    Initiatives::Initiatives()
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
               
                std::set<std::string> issues;
                Poco::Data::Session session = database::Database::get().create_session();            
                Statement select(session);
                std::string issue;
                select << "SELECT issue_key FROM Initiatives_Issue WHERE initative_name=?",
                    into(issue),
                    use(item),
                    range(0, 1); //  iterate over result set one row at a time

                while (!select.done())
                {
                    if(select.execute())
                    issues.insert(issue);
                }
                _initiatives.insert({item,issues});
            }
        }
    }
    Poco::JSON::Array::Ptr Initiatives::toJSON() const{
        Poco::JSON::Array::Ptr array = new Poco::JSON::Array();
        for(auto &init : _initiatives){
            Poco::JSON::Object::Ptr obj = new Poco::JSON::Object();
            obj->set("name",init.name);
            
            Poco::JSON::Array::Ptr issues = new Poco::JSON::Array();
            for(const std::string& str:init.issues)
                issues->add(str);
            obj->set("initiatives",issues);
            array->add(obj);
        }
            
        return array;
    }

    Initiatives &Initiatives::get()
    {
        static Initiatives instance;
        return instance;
    }

    const std::set<Initiative> &Initiatives::initiatives() const
    {
        return _initiatives;
    }

    bool Initiative::operator<(const Initiative &other) const{
        return name<other.name;

    }
} // namespace model