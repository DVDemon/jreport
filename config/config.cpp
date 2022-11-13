#include "config.h"

Config::Config()
{
}

Config &Config::get()
{
    static Config _instance;
    return _instance;
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

const std::string &Config::get_jira_username() const
{
    return _jira_username;
}
const std::string &Config::get_jira_password() const
{
    return _jira_password;
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

std::string &Config::jira_username()
{
    return _jira_username;
}
std::string &Config::jira_password()
{
    return _jira_password;
}

std::string &Config::jira_address()
{
    return _jira_address;
}