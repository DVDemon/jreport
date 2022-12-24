#include "config.h"

Config::Config()
{
}

Config &Config::get()
{
    static Config _instance;
    return _instance;
}

std::string& Config::mongo_address(){
    return _mongo_address;
}

std::string& Config::mongo_port(){
    return _mongo_port;
}

const std::string& Config::get_mongo_address(){
    return _mongo_address;
}

const std::string& Config::get_mongo_port(){
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