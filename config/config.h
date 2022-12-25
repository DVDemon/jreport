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
        std::string _jira_address;
        std::string _jira_user;
        std::string _jira_password;

        std::string _mongo_address;
        std::string _mongo_port;

        std::string _cache_servers;

    public:
        static Config& get();

        std::string& database_port();
        std::string& database_ip();
        std::string& write_request_ip();
        std::string& login();
        std::string& password();
        std::string& database();
        std::string& jira_address();
        std::string& mongo_address();
        std::string& mongo_port();
        std::string& cache_servers();

        std::string& jira_user();
        std::string& jira_password();

        const std::string& get_jira_user();
        const std::string& get_jira_password();

        const std::string& get_mongo_address();
        const std::string& get_mongo_port();

        const std::string& get_database_port() const ;
        const std::string& get_database_ip() const ;
        const std::string& get_login() const ;
        const std::string& get_password() const ;
        const std::string& get_database() const ;
        const std::string& get_jira_address() const ;
        const std::string& get_cache_servers() const;

        const std::string get_identity() const;
};

#endif