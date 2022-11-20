#include <iostream>
#include <boost/program_options.hpp>
#include <sstream>
#include "httplib.h"

#include "config/config.h"
#include "loader/jira_loader.h"
#include "model/cluster.h"
#include "model/initiative.h"
#include "model/product.h"
#include "model/comment.h"
#include "model/product_project.h"
#include "model/cluster_initiative_issue.h"
#include "model/product_initiative_issue.h"

#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

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


                httplib::Server svr;

                svr.Post("/product_project", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                         {
                                std::cout << "set project" << std::endl;
                                std::cout << req.body << std::endl;
                                Poco::JSON::Parser parser;
                                Poco::Dynamic::Var var = parser.parse(req.body);
                                Poco::JSON::Object::Ptr json = var.extract<Poco::JSON::Object::Ptr>();
                                model::ProductProject pp = model::ProductProject::fromJSON(json);
                                pp.save();

                                res.set_content("", "text/plain");
                                res.status = 200; });

                svr.Post("/comments", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                         {
                                std::cout << "set comments" << std::endl;
                                std::cout << req.body << std::endl;
                                Poco::JSON::Parser parser;
                                Poco::Dynamic::Var var = parser.parse(req.body);
                                Poco::JSON::Object::Ptr json = var.extract<Poco::JSON::Object::Ptr>();
                                model::Comment com = model::Comment::fromJSON(json);
                                com.save();

                                res.set_content("", "text/plain");
                                res.status = 200; });

                svr.Get("/comments", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                        {
                                std::cout << "get comments" << std::endl;
                                if (req.has_param("product") &&
                                    req.has_param("cluster_issue"))
                                {
                                        try
                                        {
                                                std::string product = req.get_param_value("product");
                                                std::string cluster_issue = req.get_param_value("cluster_issue");

                                                std::cout << "product:" << product << ", issue:" << cluster_issue << std::endl;

                                                model::Comment com = model::Comment::load(product, cluster_issue);
                                                std::stringstream ss;
                                                Poco::JSON::Stringifier::stringify(com.toJSON(), ss, 4, -1, Poco::JSON_PRESERVE_KEY_ORDER);
                                                res.set_content(ss.str(), "text/json; charset=utf-8");
                                        }
                                        catch (...)
                                        {
                                                res.status = 404;
                                        }
                                }
                                else
                                        res.status = 404;
                        });

                svr.Post("/product_initative_issue", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                         {
                                //std::cout << "product_initative_issue" << std::endl;
                                //std::cout << req.body << std::endl;
                                Poco::JSON::Parser parser;
                                Poco::Dynamic::Var var = parser.parse(req.body);
                                Poco::JSON::Object::Ptr json = var.extract<Poco::JSON::Object::Ptr>();
                                model::ProductInitativeIssue cii = model::ProductInitativeIssue::fromJSON(json);
                                cii.save();

                                res.set_content("", "text/plain");
                                res.status = 200; });

                svr.Get("/product_initative_epic", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                        {
                                if (req.has_param("product_issue") &&
                                    req.has_param("cluster_issue"))
                                {
                                        try
                                        {
                                                std::string product_issue = req.get_param_value("product_issue");
                                                std::string cluster_issue = req.get_param_value("cluster_issue");

                                                std::cout << "product:" << product_issue << ", issue:" << cluster_issue << std::endl;

                                                model::ProductInitativeIssue pii = model::ProductInitativeIssue::load_by_issue(product_issue, cluster_issue);
                                                std::string str;
                                                str = "{ \"issue\" : \"";
                                                str += pii.product;
                                                str += "\"}";
                                                res.set_content(str, "text/json; charset=utf-8");
                                        }
                                        catch (...)
                                        {
                                                res.status = 404;
                                        }
                                }
                                else
                                        res.status = 404;
                        });

                svr.Get("/cluster_initative_epic", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                        {
                                if (req.has_param("cluster") &&
                                    req.has_param("initiative") &&
                                    req.has_param("initiative_issue"))
                                {
                                        try
                                        {
                                                std::string cluster = req.get_param_value("cluster");
                                                std::string initiative = req.get_param_value("initiative");
                                                std::string initiative_issue = req.get_param_value("initiative_issue");

                                                model::ClusterInitativeIssue cii = model::ClusterInitativeIssue::load(cluster, initiative, initiative_issue);
                                                std::string str;
                                                str = "{ \"issue\" : \"";
                                                str += cii.issue;
                                                str += "\"}";
                                                res.set_content(str, "text/json; charset=utf-8");
                                        }
                                        catch (...)
                                        {
                                                res.status = 404;
                                        }
                                }
                                else
                                        res.status = 404;
                        });

                svr.Post("/cluster_initative_epic", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                         {
                                Poco::JSON::Parser parser;
                                Poco::Dynamic::Var var = parser.parse(req.body);
                                Poco::JSON::Object::Ptr json = var.extract<Poco::JSON::Object::Ptr>();
                                model::ClusterInitativeIssue cii = model::ClusterInitativeIssue::fromJSON(json);
                                cii.save();

                                res.set_content("", "text/plain");
                                res.status = 200; });

                svr.Get("/issue/(.*)", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                        {       
                    auto key = req.matches[1];
                    std::cout << "key:" << key << std::endl;

                    auto item = loaders::LoaderJira::get().load(key,req.get_header_value("Authorization"));
                    if(item){                       
                        std::stringstream ss;
                        Poco::JSON::Stringifier::stringify(item->toJSON(), ss, 4, -1, Poco::JSON_PRESERVE_KEY_ORDER);
                        res.set_content(ss.str(), "text/json; charset=utf-8"); 
                    } else res.status = 404; });

                svr.Get("/products", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                        {       
                        if (req.has_param("cluster")&&
                            req.has_param("cluster_issue")){
                                try{
                                        std::string cluster = req.get_param_value("cluster");
                                        std::string cluster_issue = req.get_param_value("cluster_issue");
                                        std::string str;
                                        bool comma = false;
                                        str = "[";
                                        for(auto &[name,product] : model::Products::get().products())
                                                 {
                                                        if(product->cluster == cluster){
                                                                std::string product_issue;
                                                                try{
                                                                        model::ProductInitativeIssue pii = model::ProductInitativeIssue::load(product->name,cluster_issue);
                                                                        product_issue = pii.product_issue;
                                                                }catch(...){

                                                                }
                                                                if(comma) str += ",";
                                                                        else comma = true;
                                                                str += "{ \"name\" : \""+product->name +"\", \"issue\" : \""+product_issue+"\"}";                                                                    
                                                        }
                                                }
                                                        
                                                str += "]";
                                                std::cout << str << std::endl;
                                                res.set_content(str, "text/json; charset=utf-8"); 
                                                }catch(...){
                                                res.status = 404;      
                                                }
                                        } else res.status = 404; });

                svr.Get("/product/(.*)", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                        {       
                    std::string name = req.matches[1];
                    auto &products = model::Products::get().products();

                    if(products.find(name)!=std::end(products)){
                        std::stringstream ss;
                        auto product_ptr = products[name];
                        std::optional<model::ProductProject> pp = model::ProductProject::load(name);
                        if(pp) product_ptr->project = pp->project;
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