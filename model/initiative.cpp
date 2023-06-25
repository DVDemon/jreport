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
            std::cout << "initiatives: " << array->size() << std::endl;
            for(size_t i=0;i<array->size();++i){
                Poco::JSON::Object::Ptr pitem = array->getObject(i);
                std::string item = pitem->getValue<std::string>("name");
                std::string confluence = pitem->getValue<std::string>("confluence");
               
                std::set<std::string> issues;
                Poco::Data::Session session = database::Database::get().create_session();            
                Statement select(session);
                std::string sql = database::sql_string( "SELECT issue_key FROM Initiatives_Issue WHERE initative_name=?",item);
                select << sql;
                select.execute();
                
                Poco::Data::RecordSet rs(select);
                if(rs.rowCount())
                for(auto & row : rs){
                    std::string issue;
                    issue = row["issue_key"].toString();
                    std::cout << "issue:" << issue << std::endl;
                    issues.insert(issue);
                }

                std::shared_ptr<Initiative> in_ptr = std::shared_ptr<Initiative>(new Initiative{item,confluence,issues});
                _initiatives.insert(in_ptr);
            }
        }
    }
    Poco::JSON::Array::Ptr Initiatives::toJSON() const{
        Poco::JSON::Array::Ptr array = new Poco::JSON::Array();
        for(auto &init : _initiatives){
            Poco::JSON::Object::Ptr obj = new Poco::JSON::Object();
            obj->set("name",init->name);
            
            Poco::JSON::Array::Ptr issues = new Poco::JSON::Array();
            for(const std::string& str:init->issues)
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

    std::set<std::shared_ptr<Initiative>> &Initiatives::initiatives()
    {
        return _initiatives;
    }

    bool Initiative::operator<(const Initiative &other) const{
        return name<other.name;

    }
} // namespace model