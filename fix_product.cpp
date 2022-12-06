#include <iostream>
#include <boost/program_options.hpp>
#include <sstream>

#include "config/config.h"
#include "loader/jira_loader.h"
#include "model/cluster.h"
#include "model/initiative.h"
#include "model/product.h"
#include "model/product_project.h"
#include "model/cluster_initiative_issue.h"
#include "model/product_initiative_issue.h"
#include "file_export/reports.h"

#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/Base64Encoder.h>

#include "model/cluster_project.h"
namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    try
    {
        po::options_description desc{"Options"};
        desc.add_options()("help,h", "This screen")("address,", po::value<std::string>()->required(), "set database ip address")("port,", po::value<std::string>()->required(), "databaase port")("login,", po::value<std::string>()->required(), "database login")("password,", po::value<std::string>()->required(), "database password")("database,", po::value<std::string>()->required(), "database name")("juser,", po::value<std::string>()->required(), "jira user name")("jpassword,", po::value<std::string>()->required(), "jira password")("jaddress,", po::value<std::string>()->required(), "jira address");

        po::variables_map vm;
        po::store(parse_command_line(argc, argv, desc), vm);
        std::string user, password;
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
        if (vm.count("juser"))
            user = vm["juser"].as<std::string>();
        if (vm.count("jpassword"))
            password = vm["jpassword"].as<std::string>();
        if (vm.count("jaddress"))
            Config::get().jira_address() = vm["jaddress"].as<std::string>();
        // load initiatives

        std::string token = user + ":" + password;
        std::ostringstream os;
        Poco::Base64Encoder b64in(os);
        b64in << token;
        b64in.close();
        std::string identity = "Basic " + os.str();

        model::ClusterProject::fill();
        return 1;

        for (const std::shared_ptr<model::Initiative> &initiative : model::Initiatives::get().initiatives())
        {
            for (const std::string initiative_issue_key : initiative->issues)
            {
                std::cout << "loading initiative: " << initiative_issue_key;

                std::shared_ptr<model::Issue> initiative_issue = loaders::LoaderJira::get().load(initiative_issue_key, identity);
                std::cout << " done" << std::endl;

                if (initiative_issue)
                    for (const std::string cluster : model::Cluster::get().clusters())
                    {
                        std::cout << "cluster: " << cluster << std::endl;
                        try
                        {
                            std::optional<model::ClusterInitativeIssue> cli = model::ClusterInitativeIssue::load(cluster, initiative_issue_key);
                            if (cli)
                            {
                                std::cout << "cluster initative: " << cli->issue << std::endl;
                                std::shared_ptr<model::Issue> cluster_issue = loaders::LoaderJira::get().load(cli->issue, identity);
                                if (cluster_issue)
                                {
                                    for (const model::IssueLink &link : cluster_issue->get_links())
                                    {
                                        std::shared_ptr<model::Issue> product_issue = loaders::LoaderJira::get().load(link.item->get_key(), identity);
                                        if (product_issue)
                                        {
                                            if (!product_issue->get_product().empty())
                                            {
                                                std::cout << link.item->get_key() << " [" << product_issue->get_product() << "]" << std::endl;
                                                model::ProductInitativeIssue pii{product_issue->get_product(), initiative_issue_key, link.item->get_key()};
                                                pii.save();

                                                if(!product_issue->get_project().empty()){
                                                    std::cout << "project:" << product_issue->get_project() << std::endl;
                                                    model::ProductProject pp {product_issue->get_name(),product_issue->get_project()};
                                                    pp.save();
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        catch (...)
                        {
                        }
                    }
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "exception: " << e.what() << std::endl;
    }

    return 1;
}