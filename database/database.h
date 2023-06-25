#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <memory>
#include <Poco/Data/PostgreSQL/Connector.h>
#include <Poco/Data/PostgreSQL/PostgreSQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/SessionPool.h>
#include <Poco/MongoDB/MongoDB.h>
#include <Poco/MongoDB/Connection.h>
#include <Poco/MongoDB/Database.h>

namespace database{
    class Database{
        private:
            std::string _connection_string;
            std::unique_ptr<Poco::Data::SessionPool> _pool;
            Poco::MongoDB::Connection _mongo;
            Database();
        public:
            static Database& get();
            Poco::Data::Session create_session();
            Poco::MongoDB::Connection& mongo();
    };

    template<class T> std::string sql_string(const std::string& format, T var1){

        size_t question = format.find("?");
        if(question != std::string::npos){
            std::string result;
            result = format.substr(0,question);
            result += "'"+var1+"'";
            result += format.substr(question+1);
            return result;

        } else return format;
    } 
    template<class T, class... Types> std::string sql_string(const std::string& format, T var1, Types... vars){

        size_t question = format.find("?");
        if(question != std::string::npos){
            std::string result;
            result = format.substr(0,question);
            result += "'"+var1+"'";
            std::string new_format = format.substr(question+1);
            result += sql_string(new_format,vars...);
            return result;

        } else return format;
    } 

    
}
#endif