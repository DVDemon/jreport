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
#include "file_export/xls_export.h"

#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/Base64Encoder.h>

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

        std::vector<report::Report> report;
        for (const std::shared_ptr<model::Initiative> &initiative : model::Initiatives::get().initiatives())
        {
            for (auto &[product_name, product] : model::Products::get().products())
            {
                report::Report line;
                line.initative = initiative->name;
                line.cluster = product->cluster;
                line.product = product->name;

                for (const std::string &initiative_issue : initiative->issues)
                {
                    try
                    {

                        model::ClusterInitativeIssue cii = model::ClusterInitativeIssue::load(product->cluster, initiative->name, initiative_issue);
                        model::ProductInitativeIssue pii = model::ProductInitativeIssue::load(product->name, initiative_issue);

                        auto initiative_item = loaders::LoaderJira::get().load(initiative_issue,identity);
                        if (initiative_item)
                        {
                            report::Report_Issue ii;
                            ii.key = initiative_item->get_key();
                            ii.name = initiative_item->get_name();
                            ii.status = initiative_item->get_status();

                            auto product_item = loaders::LoaderJira::get().load(pii.product_issue,identity);
                            if (product_item)
                            {
                                if(!product_item->get_project().empty()){
                                    std::cout << "project:" << product_item->get_project() << std::endl;
                                    model::ProductProject pp {product_item->get_name(),product_item->get_project()};
                                    pp.save();
                                }

                                report::Report_Issue ri;
                                ri.key = product_item->get_key();
                                ri.name = product_item->get_name();
                                ri.status = product_item->get_status();
                                if (!product_item->get_resolution().empty())
                                    ri.status = product_item->get_resolution();
                                else
                                    ri.status = product_item->get_status();

                                line.issue_status.push_back({ii, ri});

                                std::cout << initiative->name << ", " << product->cluster << ", " << product->name << initiative_issue << ", " << pii.cluster_issue << ", " << pii.product_issue << std::endl;
                            }
                        }
                    }
                    catch (...)
                    {
                    }
                }
                if (!line.issue_status.empty())
                    report.push_back(line);
            }
        }

        file_export::ExportXLS::start_export(report);
        std::cout << "Report lines count: " << report.size() << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "exception: " << e.what() << std::endl;
    }

    return 1;
}