#include <iostream>
#include <sstream>
#include <chrono>

#include "config/config.h"
#include "loader/jira_loader.h"
#include "model/cluster.h"
#include "model/initiative.h"
#include "model/product.h"
#include "model/product_project.h"
#include "model/cluster_initiative_issue.h"
#include "model/product_initiative_issue.h"
#include "model/cluster_project.h"
#include "file_export/reports.h"
#include "file_export/xls_export.h"
#include "file_export/confluence_export.h"

#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>



int main()
{
    try
    {

        std::string identity = Config::get().get_jira_identity();

        std::vector<report::Report> report;


        for (std::shared_ptr<model::Initiative> initiative : model::Initiatives::get().initiatives())
        {
            std::cout << initiative->name << std::endl;

            for (std::string initiative_issue : initiative->issues)
            {
                std::cout << "     " << initiative_issue << std::endl;

                auto initiative_item = loaders::LoaderJira::get().load(initiative_issue, identity);
                for (model::ProductInitativeIssue pi : model::ProductInitativeIssue::load_by_cluster_issue(initiative_issue)) // cii->issue))
                {
                    try
                    {
                        std::cout << "        " << pi.product << std::endl;
                        std::string cluster;

                        if (model::Products::get().products().find(pi.product) != std::end(model::Products::get().products()))
                        {
                            auto product_ptr = model::Products::get().products()[pi.product];
                            if (product_ptr)
                                cluster = product_ptr->cluster;
                        }
                        report::Report line;
                        line.initative = initiative->name;
                        line.cluster = cluster;
                        line.product = pi.product;

                        if (initiative_item)
                        {
                            report::Report_Issue ii;
                            ii.key = initiative_item->get_key();
                            ii.name = initiative_item->get_name();
                            ii.status = initiative_item->get_status();

                            auto product_item = loaders::LoaderJira::get().load(pi.product_issue, identity);
                            if (product_item)
                            {

                                product_item->save_to_mongodb();
                                if (cluster.empty())
                                {
                                    std::optional<model::ClusterProject> cp = model::ClusterProject::load(product_item->get_project());
                                    if (cp)
                                    {
                                        cluster = cp->cluster;
                                        line.cluster = cluster;
                                        std::cout << "        add cluster: " << cluster << std::endl;
                                    }
                                }

                                report::Report_Issue ri;
                                ri.key = product_item->get_key();
                                ri.name = product_item->get_name();
                                ri.status = product_item->get_status();
                                ri.assigne = product_item->get_assignee();
                                if (!product_item->get_resolution().empty())
                                    ri.status = product_item->get_resolution();
                                else
                                    ri.status = product_item->get_status();

                                for(const model::IssueLink &il : product_item->get_links()){
                                    if(il.link_type!="child of")
                                    if(il.item){
                                        ri.links.push_back(il.item->get_key());
                                    }
                                }

                                line.issue_status.push_back({ii, ri});

                                const std::vector<size_t> days {1,2,3,4,5,7,14,21,30,60,90};
                                for (size_t day : days)
                                {
                                    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
                                    now -= std::chrono::hours(day * 24);
                                    std::time_t t_t = std::chrono::system_clock::to_time_t(now);
                                    tm local_tm = *localtime(&t_t);
                                    auto old_issue = model::Issue::from_mongodb(product_item->get_key(), local_tm);
                                    if (old_issue)
                                    {
                                        ri.key = old_issue->get_key();
                                        ri.name = old_issue->get_name();
                                        ri.status = old_issue->get_status();
                                        ri.assigne = old_issue->get_assignee();
                                        ri.day_shift = day;
                                        if (!old_issue->get_resolution().empty())
                                            ri.status = old_issue->get_resolution();
                                        else
                                            ri.status = old_issue->get_status();

                                        line.issue_status.push_back({ii, ri});
                                    }
                                }
                            }
                        }
                        if (!line.issue_status.empty())
                            report.push_back(line);
                    }
                    catch (...)
                    {
                        std::cout << "exception" << std::endl;
                    }
                }
            }
        }

        std::cout << std::endl;
        std::cout << "Report lines count: " << report.size() << std::endl;
        std::cout << "Report export to xls: ";
        file_export::ExportXLS::start_export(report);
        std::cout << "Done" << std::endl;
        std::cout << "Report export to confluence: ";
        file_export::ExportConfluence::start_export(report);
        std::cout << "Done" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "exception: " << e.what() << std::endl;
    }

    return 1;
}