#include "database.h"
#include "../config/config.h"

namespace database{
    Database::Database(){
        _connection_string+="host=";
        _connection_string+=Config::get().get_database_ip();
        _connection_string+=" port=";
        _connection_string+=Config::get().get_database_port();
        _connection_string+=" user=";
        _connection_string+=Config::get().get_login();
        _connection_string+=" dbname=";
        _connection_string+=Config::get().get_database();
        _connection_string+=" password=";
        _connection_string+=Config::get().get_password();
        std::cout << "[database connection string]:"<< _connection_string << std::endl;


        Poco::Data::PostgreSQL::Connector::registerConnector();

        //_pool = std::make_unique<Poco::Data::SessionPool>(Poco::Data::PostgreSQL::Connector::KEY, _connection_string);

        // mongodb://<user>:<password>@hostname.com:<port>/database-name
        _mongo.connect(Config::get().get_mongo_address(),std::atol(Config::get().mongo_port().c_str()));
    }
    Poco::MongoDB::Connection& Database::mongo(){
        return _mongo;
    }

    Database& Database::get(){
        static Database _instance;
        return _instance;
    }

    Poco::Data::Session Database::create_session(){
        return  Poco::Data::SessionFactory::instance().create(
            Poco::Data::PostgreSQL::Connector::KEY, _connection_string);
        //Poco::Data::Session(_pool->get());
    }

}
