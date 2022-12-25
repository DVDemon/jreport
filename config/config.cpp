#include "config.h"
#include <Poco/Base64Encoder.h>
#include <iostream>
#include <sstream>

Config::Config()
{

    _database_ip = std::getenv("DATABASE_IP");
    _database_port = std::getenv("DATABASE_PORT");
    _login = std::getenv("DATABASE_LOGIN");
    _password = std::getenv("DATABASE_PASSWORD");
    _database = std::getenv("DATABASE_NAME");
    _jira_address = std::getenv("JIRA_ADDRESS");
    _mongo_address = std::getenv("MONGO_ADDRESS");
    ;
    _mongo_port = std::getenv("MONGO_PORT");
    _cache_servers = std::getenv("CACHE");
    _jira_user = std::getenv("JIRA_USER");
    _jira_password = std::getenv("JIRA_PASSWORD");
}

const std::string Config::get_identity() const
{
    std::string token = _jira_user + ":" + _jira_password;
    std::ostringstream os;
    Poco::Base64Encoder b64in(os);
    b64in << token;
    b64in.close();
    std::string identity = "Basic " + os.str();
    return identity;
}

std::string &Config::jira_user()
{
    return _jira_user;
}

std::string &Config::jira_password()
{
    return _jira_password;
}

const std::string &Config::get_jira_user()
{
    return _jira_user;
}

const std::string &Config::get_jira_password()
{
    return _jira_password;
}

Config &Config::get()
{
    static Config _instance;
    return _instance;
}

std::string &Config::mongo_address()
{
    return _mongo_address;
}

std::string &Config::mongo_port()
{
    return _mongo_port;
}

const std::string &Config::get_cache_servers() const
{
    return _cache_servers;
}

std::string &Config::cache_servers()
{
    return _cache_servers;
}

const std::string &Config::get_mongo_address()
{
    return _mongo_address;
}

const std::string &Config::get_mongo_port()
{
    return _mongo_port;
}

const std::string &Config::get_database_port() const
{
    return _database_port;
}

const std::string &Config::get_database_ip() const
{
    return _database_ip;
}

const std::string &Config::get_login() const
{
    return _login;
}

const std::string &Config::get_password() const
{
    return _password;
}
const std::string &Config::get_database() const
{
    return _database;
}

const std::string &Config::get_jira_address() const
{
    return _jira_address;
}

std::string &Config::database_port()
{
    return _database_port;
}

std::string &Config::database_ip()
{
    return _database_ip;
}

std::string &Config::login()
{
    return _login;
}

std::string &Config::password()
{
    return _password;
}

std::string &Config::database()
{
    return _database;
}

std::string &Config::jira_address()
{
    return _jira_address;
}