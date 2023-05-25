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
#include "model/hrefs.h"
#include "file_export/reports.h"
#include "file_export/xls_export.h"
#include "file_export/confluence_export.h"

#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/URI.h>
#include <thread>
#include <chrono>

using namespace std::chrono;


#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include "database/database.h"
using namespace Poco::Data::Keywords;

bool do_init(){

    try{
        Poco::Data::Session session = database::Database::get().create_session();
        Poco::Data::Statement create_stmt(session);
        std::cout << "create tables" << std::endl;
        create_stmt << "CREATE TABLE IF NOT EXISTS Issue (id VARCHAR(256) NOT NULL,key_field VARCHAR(256) NOT NULL,name VARCHAR(256) NOT NULL,description VARCHAR(4096) NOT NULL,author VARCHAR(256) NOT NULL,assignee VARCHAR(256) NOT NULL,status VARCHAR(256) NOT NULL,project VARCHAR(256) NOT NULL,PRIMARY KEY (id),KEY(key_field));";create_stmt.execute();
        create_stmt << "CREATE TABLE IF NOT EXISTS Initiatives_Issue(initative_name VARCHAR(256) NOT NULL,issue_key VARCHAR(256) NOT NULL);";create_stmt.execute();
        create_stmt << "CREATE TABLE IF NOT EXISTS Cluster_Initiative_Issue(issue varchar(256) NOT NULL,initiative_issue VARCHAR(256) NOT NULL,initiative VARCHAR(256) NOT NULL,cluster VARCHAR(256) NOT NULL);";create_stmt.execute();
        create_stmt << "CREATE TABLE IF NOT EXISTS Product_Initiative_Issue(product varchar(256) NOT NULL,cluster_issue VARCHAR(256) NOT NULL,product_issue VARCHAR(256) NOT NULL);";create_stmt.execute();
        create_stmt << "CREATE TABLE IF NOT EXISTS Product_Initiative_Comment(product VARCHAR(256) NOT NULL,cluster_issue VARCHAR(256) NOT NULL,comment VARCHAR(4096) NOT NULL,address VARCHAR(4096) NOT NULL);";create_stmt.execute();
        create_stmt << "CREATE TABLE IF NOT EXISTS Product_Project(product VARCHAR(256) NOT NULL,project VARCHAR(256) NOT NULL);";create_stmt.execute();
        create_stmt << "CREATE TABLE IF NOT EXISTS Cluster_Project(cluster VARCHAR(256) NOT NULL,project VARCHAR(256) NOT NULL);";create_stmt.execute();
        create_stmt << "CREATE TABLE IF NOT EXISTS HREF(link VARCHAR(512) NOT NULL,title VARCHAR(512), KEY(link));";create_stmt.execute();
        
        std::vector<std::string> inserts{
            "INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('Cloud Native','KA-1395');",
            "INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('Cloud Native','KA-1396');",
            "INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('Cloud Native','KA-1397');",
            "INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('ArchOPS','KA-1435');",
            "INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('TechRadar','ARC-199');",
            "INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('Target Architecture Guidelines','KA-1140');",
            "INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('Target Architecture Guidelines','KA-1141');",
            "INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('Target Architecture Guidelines','KA-1142');",
            "INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('Near Realtime','KA-1480');",
            "INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('Near Realtime','KA-1504');",
            "INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('Micro-frontends for Web UIs','KA-2234');",
            "INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('Пилотирование практики Architecture as Code','KA-2227');",
            "INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('Запрет интеграции через БД','KA-2217');",
            "INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('Реализация задач по переходу с технологий в статусе HOLD - 23Q1-23Q4','ARC-200');",
            "INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('[BCAA] Актуализация описания компетенций и связей с ПО','KA-2255');"
        };
        std::cout << "check records" << std::endl;
        long count{0};
        Poco::Data::Statement select(session);
        select << "SELECT count(*) as cnt FROM Initiatives_Issue",
                into(count),
                range(0, 1); //  iterate over result set one row at a time

        Poco::Data::RecordSet rs(select);
        rs.moveFirst();
        if(count==0){
            std::cout << "insert config" << std::endl;
            for(auto s:inserts){
                Poco::Data::Statement ins_stmt(session);
                ins_stmt << s;
                ins_stmt.execute();

            }
         } else std::cout << "config already done" << std::endl;
        


    }catch(std::exception &ex){
        std::cout << "init exception:" << ex.what()<< std::endl;
        return false;
    }
    return true;
}

void do_hrefs()
{
    for (const model::ProductInitativeIssue &pii : model::ProductInitativeIssue::load_all())
    {

        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
        std::time_t t_t = std::chrono::system_clock::to_time_t(now);
        tm local_tm = *localtime(&t_t);
        std::shared_ptr<model::Issue> i = model::Issue::from_mongodb(pii.product_issue, local_tm);
        if (i)
        {
            if (!i->get_hrefs().empty())
            {
                for (std::string link : i->get_hrefs())
                {

                    try
                    {
                        if (link.find("confluence")!= std::string::npos)
                        {
                            
                            std::cout << "   " + link;
                            
                            if (!model::HREFS::get()[link])
                            {
                                std::this_thread::sleep_for(1000ms);
                                Poco::URI Uri(link);
                                Poco::URI::QueryParameters qp = Uri.getQueryParameters();
                                std::string pageId;
                                for (const auto &[n, v] : qp)
                                {
                                    if (n == "pageId")
                                        pageId = v;
                                }
                                if (!pageId.empty())
                                {
                                    std::string url = Config::get().get_confluence_address() + "/content/" + pageId;
                                    auto res = file_export::ExportConfluence::get_page_by_url(url);
                                    if (res)
                                    {
                                        std::string title = res->second;
                                        std::cout << "... title:" << title << std::endl;
                                        model::HREFS::get().set(link, title);
                                    } else std::cout << " ... fail load confluence" << std::endl;
                                } else std::cout << " ... fail get page" << std::endl;
                            } else std::cout << " ... from cache" << std::endl;
                        }
                    }
                    catch (std::exception &ex)
                    {
                        std::cout << "hrefs exception:" << ex.what() << std::endl;
                    }
                }
            }
        }
        
    }
}

void do_fix_product()
{
    try
    {
        std::string identity = Config::get().get_jira_identity();

        for (const std::shared_ptr<model::Initiative> &initiative : model::Initiatives::get().initiatives())
        {
            for (const std::string &initiative_issue_key : initiative->issues)
            {
                std::cout << "loading initiative: " << initiative_issue_key;

                std::shared_ptr<model::Issue> initiative_issue = loaders::LoaderJira::get().load(initiative_issue_key, identity);
                std::cout << " done" << std::endl;

                if (initiative_issue)
                    for (const std::string &cluster : model::Cluster::get().clusters())
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

                                                if (!product_issue->get_project().empty())
                                                {
                                                    std::cout << "project:" << product_issue->get_project() << std::endl;
                                                    model::ProductProject pp{product_issue->get_product(), product_issue->get_project()};
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
}
void do_export()
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
                        std::cout << "        " << pi.product << ", " << pi.product_issue << std::endl;
                        std::string cluster;

                        if (model::Products::get().products().find(pi.product) != std::end(model::Products::get().products()))
                        {
                            auto product_ptr = model::Products::get().products()[pi.product];
                            if (product_ptr)
                                cluster = product_ptr->cluster;
                        }
                        else
                        {
                            if (initiative_item)
                            {
                                std::string project = initiative_item->get_project();
                                std::optional<model::ClusterProject> cp = model::ClusterProject::load(project);
                                if (cp)
                                {
                                    cluster = cp->cluster;
                                }
                            }
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
                            ii.fixed_version = initiative_item->get_fixed_version();
                            ii.issue = initiative_item;

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
                                    else
                                        std::cout << "------------ can't get cluster for project [" << product_item->get_project() << "]" << std::endl;
                                }

                                report::Report_Issue ri;
                                ri.key = product_item->get_key();
                                ri.name = product_item->get_name();
                                ri.status = product_item->get_status();
                                ri.assigne = product_item->get_assignee();
                                ri.fixed_version = product_item->get_fixed_version();
                                ri.issue = product_item;
                                if (!product_item->get_resolution().empty())
                                    ri.status = product_item->get_resolution();
                                else
                                    ri.status = product_item->get_status();

                                for (const model::IssueLink &il : product_item->get_links())
                                {
                                    if (il.link_type != "child of")
                                        if (il.item)
                                        {
                                            ri.links.push_back({il.item->get_key(), il.link_type});
                                        }
                                }

                                line.issue_status.push_back({ii, ri});

                                const std::set<size_t> days{1, 2, 3, 4, 5, 7, 14, 21, 30, 60, 90};
                                size_t status_changed = 0;
                                const std::string cur_status = product_item->get_status();

                                for (size_t day = 1; day <= 90; day++)
                                {

                                    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
                                    now -= std::chrono::hours(day * 24);
                                    std::time_t t_t = std::chrono::system_clock::to_time_t(now);
                                    tm local_tm = *localtime(&t_t);
                                    auto old_issue = model::Issue::from_mongodb(product_item->get_key(), local_tm);
                                    if (old_issue)
                                    {
                                        // std::cout << "*";
                                        if (status_changed == 0)
                                        {
                                            if (cur_status != old_issue->get_status())
                                                status_changed = day;
                                        }
                                        if (days.find(day) != std::end(days))
                                        {
                                            ri.key = old_issue->get_key();
                                            ri.name = old_issue->get_name();
                                            ri.status = old_issue->get_status();
                                            ri.assigne = old_issue->get_assignee();
                                            ri.fixed_version = old_issue->get_fixed_version();
                                            ri.issue = old_issue;
                                            ri.day_shift = day;
                                            if (!old_issue->get_resolution().empty())
                                                ri.status = old_issue->get_resolution();
                                            else
                                                ri.status = old_issue->get_status();

                                            line.issue_status.push_back({ii, ri});
                                        }
                                    } // else break;
                                }
                                if (status_changed != 0)
                                    line.issue_status[0].second.status_changed = status_changed;
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
}

int main()
{

    while(!do_init()) std::this_thread::sleep_for(15*1000ms);

    while(true){
        try{
            do_fix_product();
            do_hrefs();
            do_export();
        }catch(std::exception &ex)
        {
            std::cout << "exception: " << ex.what() << std::endl;
        }
        std::this_thread::sleep_for(60min); // an hour
    }
    return 1;
}