#include <iostream>
#include <boost/program_options.hpp>

#include "web_server/http_web_server.h"
#include "config/config.h"

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    try
    {
        po::options_description desc{"Options"};
        desc.add_options()("help,h", "This screen")("address,", po::value<std::string>()->required(), "set database ip address")("port,", po::value<std::string>()->required(), "databaase port")("login,", po::value<std::string>()->required(), "database login")("password,", po::value<std::string>()->required(), "database password")("database,", po::value<std::string>()->required(), "database name")("juser,", po::value<std::string>()->required(), "jira user name")("jpassword,", po::value<std::string>()->required(), "jira password")("jaddress,", po::value<std::string>()->required(), "jira address");

        po::variables_map vm;
        po::store(parse_command_line(argc, argv, desc), vm);

        if (vm.count("help"))
            std::cout << desc << '\n';
        if (vm.count("address"))
            Config::get().database_ip() = vm["address"].as<std::string>();
        if (vm.count("port"))
            Config::get().database_port() = vm["port"].as<std::string>();
        if (vm.count("login"))
            Config::get().login() = vm["login"].as<std::string>();
        if (vm.count("password"))
            Config::get().password() = vm["password"].as<std::string>();
        if (vm.count("database"))
            Config::get().database() = vm["database"].as<std::string>();
        if (vm.count("jaddress"))
            Config::get().jira_address() = vm["jaddress"].as<std::string>();

        HTTPWebServer app;
        return app.run(argc, argv);
    }
    catch (const std::exception &e)
    {
        std::cerr << "exception: " << e.what() << std::endl;
    }
}