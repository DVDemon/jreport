#include <iostream>
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


int main()
{
    try
    {
        std::string identity = Config::get().get_jira_identity();

        for (const std::shared_ptr<model::Initiative> &initiative : model::Initiatives::get().initiatives())
        {
            for (const std::string & initiative_issue_key : initiative->issues)
            {
                std::cout << "loading initiative: " << initiative_issue_key;

                std::shared_ptr<model::Issue> initiative_issue = loaders::LoaderJira::get().load(initiative_issue_key, identity);
                std::cout << " done" << std::endl;

                if (initiative_issue)
                    for (const std::string & cluster : model::Cluster::get().clusters())
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
                                                    model::ProductProject pp {product_issue->get_product(),product_issue->get_project()};
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
    
        model::ClusterProject::fill();
    }
    catch (const std::exception &e)
    {
        std::cerr << "exception: " << e.what() << std::endl;
    }

    return 1;
}