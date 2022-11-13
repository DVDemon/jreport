#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class  Config{
    private:
        Config();
        std::string _database_ip;
        std::string _database_port;
        std::string _login;
        std::string _password;
        std::string _database;
        std::string _jira_username;
        std::string _jira_password;
        std::string _jira_address;


    public:
        static Config& get();

        std::string& database_port();
        std::string& database_ip();
        std::string& write_request_ip();
        std::string& login();
        std::string& password();
        std::string& database();
        std::string& jira_username();
        std::string& jira_password();
        std::string& jira_address();

        const std::string& get_database_port() const ;
        const std::string& get_database_ip() const ;
        const std::string& get_login() const ;
        const std::string& get_password() const ;
        const std::string& get_database() const ;
        const std::string& get_jira_username() const ;
        const std::string& get_jira_password() const ;
        const std::string& get_jira_address() const ;
};

#endif