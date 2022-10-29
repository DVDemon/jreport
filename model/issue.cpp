#include "issue.h"
#include "../database/database.h"

#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace model
{
    std::string &Issue::id() {
        return _id;
    }

    std::string &Issue::key() {
        return _key;
    }

    std::string &Issue::name() {
        return _name;
    }
    std::string &Issue::description() {
        return _description;
    }
    std::string &Issue::author() {
        return _author;
    }
    std::string &Issue::assignee() {
        return _assignee;
    }
    std::string &Issue::status() {
        return _status;
    }

    const std::string &Issue::get_id() {
        return _id;
    }

    const std::string &Issue::get_key() {
        return _key;
    }
    const std::string &Issue::get_name() {
        return _name;
    }
    const std::string &Issue::get_description() {
        return _description;
    }
    const std::string &Issue::get_author() {
        return _author;
    }
    const std::string &Issue::get_assignee() {
        return _assignee;
    }
    const std::string &Issue::get_status() {
        return _status;
    }

    void Issue::init() {

    }

    Issue Issue::read_by_id(std::string &id) {
       try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Issue> result;
            Issue a;
            select << "SELECT id,key_field,name,description,author,assignee,status FROM Issue WHERE id = ?",
                into(a._id),
                into(a._key),
                into(a._name),
                into(a._description),
                into(a._author),
                into(a._assignee),
                into(a._status),
                use(id),
                range(0, 1);

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (!rs.moveFirst()) throw std::logic_error("not found");

            return a;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }
    
    std::vector<Issue> Issue::read_all() {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Issue> result;
            Issue a;
            select << "SELECT id,key_field,name,description,author,assignee,status FROM Issue",
                into(a._id),
                into(a._key),
                into(a._name),
                into(a._description),
                into(a._author),
                into(a._assignee),
                into(a._status),
                range(0, 1); //  iterate over result set one row at a time

            while (!select.done())
            {
                if(select.execute())
                result.push_back(a);
            }
            return result;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }
    
    void Issue::save_to_mysql() {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO Author (id,key,name,description,author,assignee,status) VALUES(?, ?, ?, ?, ?, ?)",
                use(_id),
                use(_key),
                use(_name),
                use(_description),
                use(_author),
                use(_assignee),
                use(_status);

            insert.execute();
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }

    }

    Issue Issue::fromJSON(const std::string &str) {
        Issue issue;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        issue.id()          = object->getValue<std::string>("id");
        issue.key()        = object->getValue<std::string>("key");
        issue.name()        = object->getValue<std::string>("name");
        issue.description() = object->getValue<std::string>("description");
        issue.author()      = object->getValue<std::string>("author");
        issue.assignee()    = object->getValue<std::string>("assignee");
        issue.status()      = object->getValue<std::string>("status");

        return issue;
    }

    Poco::JSON::Object::Ptr Issue::toJSON() const {

        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("id", _id);
        root->set("key", _key);
        root->set("name", _name);
        root->set("description", _description);
        root->set("author", _author);
        root->set("assignee", _assignee);
        root->set("status", _status);
        return root;
    }
}

std::ostream & operator<<(std::ostream& os,model::Issue& issue){
    os << "id:" << issue.get_id() << std::endl;
    os << "key:" << issue.get_key() << std::endl;
    os << "name:" << issue.get_name() << std::endl;
    os << "description:" << issue.get_description() << std::endl;
    os << "author:" << issue.get_author() << std::endl;
    os << "get_assignee:" << issue.get_assignee() << std::endl;
    os << "status:" << issue.get_status() << std::endl;

    return os;
}
