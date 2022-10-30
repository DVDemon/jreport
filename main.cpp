#include <iostream>
#include <boost/program_options.hpp>
#include <sstream>
#include "httplib.h"

#include "config/config.h"
#include "loader/jira_loader.h"
#include "model/cluster.h"
#include "model/initiative.h"
#include "model/product.h"

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    try
    {
        po::options_description desc{"Options"};
        desc.add_options()("help,h", "This screen")
        ("address,", po::value<std::string>()->required(), "set database ip address")
        ("port,", po::value<std::string>()->required(), "databaase port")
        ("login,", po::value<std::string>()->required(), "database login")
        ("password,", po::value<std::string>()->required(), "database password")
        ("database,", po::value<std::string>()->required(), "database name");

        po::variables_map vm;
        po::store(parse_command_line(argc, argv, desc), vm);

        if (vm.count("help")) std::cout << desc << '\n';
        if (vm.count("address")) Config::get().database_ip() = vm["address"].as<std::string>();
        if (vm.count("port")) Config::get().database_port() = vm["port"].as<std::string>();
        if (vm.count("login")) Config::get().login() = vm["login"].as<std::string>();
        if (vm.count("password")) Config::get().password() = vm["password"].as<std::string>();
        if (vm.count("database")) Config::get().database() = vm["database"].as<std::string>();


        httplib::Server svr;

        svr.Get("/issue/(.*)", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                {       
                    auto numbers = req.matches[1];
                    std::string request_uri {"http://jira.mts.ru/rest/api/2/issue/"};
                    request_uri+=numbers;
                    auto item = loaders::LoaderJira::get().load(request_uri);
                    if(item){                       
                        std::stringstream ss;
                        Poco::JSON::Stringifier::stringify(item->toJSON(), ss, 4, -1, Poco::JSON_PRESERVE_KEY_ORDER);
                        res.set_content(ss.str(), "text/json; charset=utf-8"); 
                    }
                });

        svr.Get("/products",[]([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                {       
          
                        std::stringstream ss;
                        Poco::JSON::Stringifier::stringify(model::Products::get().toJSON(), ss, 4, -1, Poco::JSON_PRESERVE_KEY_ORDER);
                        res.set_content(ss.str(), "text/json; charset=utf-8"); 
                });

        svr.Get("/clusters",[]([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                {       
          
                        std::stringstream ss;
                        Poco::JSON::Stringifier::stringify(model::Cluster::get().toJSON(), ss, 4, -1, Poco::JSON_PRESERVE_KEY_ORDER);
                        res.set_content(ss.str(), "text/json; charset=utf-8"); 
                });
        
        svr.Get("/initiatives",[]([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                {       
          
                        std::stringstream ss;
                        Poco::JSON::Stringifier::stringify(model::Initiatives::get().toJSON(), ss, 4, -1, Poco::JSON_PRESERVE_KEY_ORDER);
                        res.set_content(ss.str(), "text/json; charset=utf-8"); 
                });
    /*    svr.Get("/body-header-param", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                {
                    if (req.has_header("Content-Length")) {
                        auto val = req.get_header_value("Content-Length");
                        
                        }
        
                        if (req.has_param("key")) {
                            auto val = req.get_param_value("key");
                            res.set_content(val, "text/plain"); 
                        } else
                        res.set_content(req.body, "text/plain"); 
                });*/

        svr.Get("/stop", [&]([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                { 
                    svr.stop(); 
                });

        std::cout << "starting server at port 80 ..." << std::endl;
        svr.listen("0.0.0.0", 80);
    }
    catch (const std::exception &e)
    {
        std::cerr << "exception: " <<e.what() << std::endl;
    }

    return 1;
}