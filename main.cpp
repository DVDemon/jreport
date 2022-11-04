#include <iostream>
#include <boost/program_options.hpp>
#include <sstream>
#include "httplib.h"

#include "config/config.h"
#include "loader/jira_loader.h"
#include "model/cluster.h"
#include "model/initiative.h"
#include "model/product.h"
#include "model/cluster_initiative_issue.h"

#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
        try
        {
                po::options_description desc{"Options"};
                desc.add_options()("help,h", "This screen")("address,", po::value<std::string>()->required(), "set database ip address")("port,", po::value<std::string>()->required(), "databaase port")("login,", po::value<std::string>()->required(), "database login")("password,", po::value<std::string>()->required(), "database password")("database,", po::value<std::string>()->required(), "database name");

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

                httplib::Server svr;

                svr.Post("/cluster_initative_epic", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                         {
                                std::cout << "cluster_initative_epic" << std::endl;
                                std::cout << req.body << std::endl;
                                Poco::JSON::Parser parser;
                                Poco::Dynamic::Var var = parser.parse(req.body);
                                Poco::JSON::Object::Ptr json = var.extract<Poco::JSON::Object::Ptr>();
                                model::ClusterInitativeIssue cii = model::ClusterInitativeIssue::fromJSON(json);
                                cii.save();

                                res.set_content("", "text/plain");
                                res.status = 200;
                         });

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
                    } else res.status = 404; });

                svr.Get("/product/(.*)", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                        {       
                    std::string name = req.matches[1];
                    auto &products = model::Products::get().products();

                    if(products.find(name)!=std::end(products)){
                        std::stringstream ss;
                        auto product_ptr = products[name];
                        Poco::JSON::Stringifier::stringify(product_ptr->toJSON(), ss, 4, -1, Poco::JSON_PRESERVE_KEY_ORDER);
                        res.set_content(ss.str(), "text/json; charset=utf-8"); 
                    } else res.status = 404; });

                svr.Get("/products", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                        {       
          
                        std::stringstream ss;
                        Poco::JSON::Stringifier::stringify(model::Products::get().toJSON(), ss, 4, -1, Poco::JSON_PRESERVE_KEY_ORDER);
                        res.set_content(ss.str(), "text/json; charset=utf-8"); });

                svr.Get("/clusters", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                        {       
          
                        std::stringstream ss;
                        Poco::JSON::Stringifier::stringify(model::Cluster::get().toJSON(), ss, 4, -1, Poco::JSON_PRESERVE_KEY_ORDER);
                        res.set_content(ss.str(), "text/json; charset=utf-8"); });

                svr.Get("/initiatives", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                        {       
          
                        std::stringstream ss;
                        Poco::JSON::Stringifier::stringify(model::Initiatives::get().toJSON(), ss, 4, -1, Poco::JSON_PRESERVE_KEY_ORDER);
                        res.set_content(ss.str(), "text/json; charset=utf-8"); });

                svr.Get("/stop", [&]([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                        { svr.stop(); });

                std::cout << "starting server at port 80 ..." << std::endl;
                svr.listen("0.0.0.0", 80);
        }
        catch (const std::exception &e)
        {
                std::cerr << "exception: " << e.what() << std::endl;
        }

        return 1;
}