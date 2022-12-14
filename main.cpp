#include <iostream>
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

int main()
{
        try
        {
                httplib::Server svr;

                svr.Post("/product_project", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                         {
                                std::cout << "set project" << std::endl;
                                std::cout << "[" << req.body << "]" << std::endl;
                                if(req.body.empty()) {
                                        res.status = 404;
                                        return 404;
                                }

                                Poco::JSON::Parser parser;
                                Poco::Dynamic::Var var = parser.parse(req.body);
                                Poco::JSON::Object::Ptr json = var.extract<Poco::JSON::Object::Ptr>();
                                model::ProductProject pp = model::ProductProject::fromJSON(json);
                                pp.save();

                                res.set_content("", "text/plain");
                                res.status = 200;
                                return 200; });

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

                                                std::optional<model::Comment> com = model::Comment::load(product, cluster_issue);
                                                if(com){
                                                std::stringstream ss;
                                                Poco::JSON::Stringifier::stringify(com->toJSON(), ss, 4, -1, Poco::JSON_PRESERVE_KEY_ORDER);
                                                res.set_content(ss.str(), "text/json; charset=utf-8");
                                                return 200;
                                                } else {
                                                        std::cout << "comment not found" << std::endl;
                                                        return 404;
                                                }
                                        }
                                        catch (...)
                                        {
                                                res.status = 404;
                                        }
                                }
                                else
                                        res.status = 404;
                                return 404; });

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

                svr.Get("/issues_with_products/(.*)", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                        {
                                auto key = req.matches[1];
                                if (req.has_param("initiative_epic")){
                                        std::string initiative_epic = req.get_param_value("initiative_epic");
                                        std::cout << "initiative_epic:" << initiative_epic << std::endl;
                                        try
                                        {
                                                std::string cluster_issue = key;
                                                auto item = loaders::LoaderJira::get().load(cluster_issue,req.get_header_value("Authorization"));
                                                if(item){ 
                                                        struct result_t{
                                                                std::string product_issue;
                                                                std::string product_name;
                                                                std::string product_issue_name;
                                                        };
                                                        std::vector<result_t> res_array;

                                                        for(model::IssueLink& lnk : item->links()){
                                                                if((lnk.link_type=="parent of")&&(lnk.item)){
                                                                        result_t r;
                                                                        r.product_issue = lnk.item->get_key();
                                                                        r.product_issue_name = lnk.item->get_name();
                                                                        std::cout << "product_issue: [" << r.product_issue << "] cluster_issue:["<< cluster_issue <<"]"<<std::endl;
                                                                        std::optional<model::ProductInitativeIssue>  pii = model::ProductInitativeIssue::load_by_issue(r.product_issue, initiative_epic);
                                                                        if(pii) {
                                                                                std::cout << "product_name: " << pii->product << std::endl;
                                                                                r.product_name = pii->product;
                                                                        }
                                                                        res_array.push_back(r);
                                                                }                           
                                                        }

                                                        Poco::JSON::Array::Ptr links_array = new Poco::JSON::Array();
                                                        for(size_t i=0;i<res_array.size();++i){
                                                                auto &r = res_array[i];
                                                                Poco::JSON::Object::Ptr l =  new Poco::JSON::Object();
                                                                l->set("product_issue",r.product_issue);
                                                                l->set("product_issue_name", r.product_issue_name);
                                                                l->set("product_name", r.product_name);
                                                                links_array->add(l);
                                                        }

                                                        std::stringstream ss;
                                                        Poco::JSON::Stringifier::stringify(links_array, ss, 4, -1, Poco::JSON_PRESERVE_KEY_ORDER);
                                                        res.set_content(ss.str(), "text/json; charset=utf-8"); 
                                                        return 200;
                                                }                                                
                                        }
                                        catch (...)
                                        {
                                                res.status = 404;
                                        }
                                }
                               
                                        return 404; });

                svr.Get("/product_initative_issue", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                        {
                                if (req.has_param("product_issue") &&
                                    req.has_param("cluster_issue"))
                                {

                                                std::string product_issue = req.get_param_value("product_issue");
                                                std::string cluster_issue = req.get_param_value("cluster_issue");

                                                std::cout << "product:" << product_issue << ", issue:" << cluster_issue << std::endl;

                                                std::optional<model::ProductInitativeIssue>  pii = model::ProductInitativeIssue::load_by_issue(product_issue, cluster_issue);

                                                
                                                if(pii){
                                                std::string str;
                                                str = "{ \"issue\" : \"";
                                                str += pii->product;
                                                str += "\"}";
                                                res.set_content(str, "text/json; charset=utf-8");
                                                return 200;
                                                } else {
                                                        res.status = 404;
                                                        return 404;
                                                }

                                }
                                
                                res.status = 404; 
                                return 404; 
                        });

                svr.Get("/cluster_initative_epic", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                        {
                                if (req.has_param("cluster") &&
                                    req.has_param("initiative_issue"))
                                {
                                        try
                                        {
                                                std::string cluster = req.get_param_value("cluster");
                                                std::string initiative_issue = req.get_param_value("initiative_issue");

                                                std::cout << "get:" << cluster << ", "  << initiative_issue << std::endl;

                                                std::optional<model::ClusterInitativeIssue> cii = model::ClusterInitativeIssue::load(cluster, initiative_issue);
                                                if(!cii) {
                                                        std::cout << "not found" << std::endl;
                                                        res.status = 404;
                                                        return res.status;      
                                                }
                                                std::string str;
                                                str = "{ \"issue\" : \"";
                                                str += cii->issue;
                                                str += "\"}";
                                                res.status = 200;
                                                res.set_content(str, "text/json; charset=utf-8");
                                        }
                                        catch (...)
                                        {
                                                std::cout << "exception" << std::endl;
                                                res.status = 404;
                                                return res.status;
                                        }
                                }
                                else
                                        res.status = 404;
                                return res.status; });

                svr.Post("/cluster_initative_epic", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                         {
                                std::cout << "cloud_initiative_epic" << std::endl;
                                Poco::JSON::Parser parser;
                                Poco::Dynamic::Var var = parser.parse(req.body);
                                Poco::JSON::Object::Ptr json = var.extract<Poco::JSON::Object::Ptr>();
                                model::ClusterInitativeIssue cii = model::ClusterInitativeIssue::fromJSON(json);
                                Poco::JSON::Stringifier::stringify(json, std::cout, 4, -1, Poco::JSON_PRESERVE_KEY_ORDER);
                                std::cout << std::endl;
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
                        return 200;
                    } else return 404; });

                svr.Get("/products", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                        {       
                        if (req.has_param("cluster")&&
                            req.has_param("cluster_issue")){
                                try{
                                        std::string cluster = req.get_param_value("cluster");
                                        std::string cluster_issue = req.get_param_value("cluster_issue");
                                        Poco::JSON::Array::Ptr array = new Poco::JSON::Array();
                                        for(auto &[name,product] : model::Products::get().products())
                                                 {
                                                        if(product->cluster == cluster){
                                                                std::string product_issue;
                                                                try{
                                                                        model::ProductInitativeIssue pii = model::ProductInitativeIssue::load(product->name,cluster_issue);
                                                                        product_issue = pii.product_issue;
                                                                }catch(...){

                                                                }
                                                                Poco::JSON::Object::Ptr l =  new Poco::JSON::Object();
                                                                l->set("name",product->name);
                                                                l->set("issue", product_issue);
                                                                array->add(l);                                                           
                                                        }
                                                }

                                                std::stringstream ss;
                                                Poco::JSON::Stringifier::stringify(array, ss, 4, -1, Poco::JSON_PRESERVE_KEY_ORDER);
                                                res.set_content(ss.str(), "text/json; charset=utf-8"); 
                                                res.status = 200; 
                                                }catch(...){
                                                res.status = 404;      
                                                }
                                        } else res.status = 404; });


                svr.Get("/clusters", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                        {       
          
                        std::stringstream ss;
                        Poco::JSON::Stringifier::stringify(model::Cluster::get().toJSON(), ss, 4, -1, Poco::JSON_PRESERVE_KEY_ORDER);
                        res.set_content(ss.str(), "text/json; charset=utf-8"); });

                svr.Get("/initiatives", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                        {       
                        std::cout << "initiatives ...";
                        std::stringstream ss;
                        Poco::JSON::Stringifier::stringify(model::Initiatives::get().toJSON(), ss, 4, -1, Poco::JSON_PRESERVE_KEY_ORDER);
                        res.set_content(ss.str(), "text/json; charset=utf-8"); 
                        std::cout << "done" << std::endl;; });

                svr.Get("/report", []([[maybe_unused]] const httplib::Request &req, [[maybe_unused]] httplib::Response &res)
                        {       
                                std::ifstream ifs("report.xlsx", std::ifstream::binary);
                                if (ifs.is_open())
                                {
                                        ifs.seekg (0, ifs.end);
                                        int length = ifs.tellg();
                                        ifs.seekg (0, ifs.beg);

                                        char * buffer = new char [length];

                                        std::cout << "Reading " << length << " characters... ";
                                        // read data as a block:
                                        ifs.read (buffer,length);

                                        if (ifs)
                                                std::cout << "all characters read successfully.";
                                        else
                                                std::cout << "error: only " << ifs.gcount() << " could be read";
                                        ifs.close();

                                        // ...buffer contains the entire file...
                                        std::string str(buffer,length);
                                        res.set_content(str,"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");

                                        delete[] buffer;
                                };
                                return 200;
                        });

                std::cout << "starting server at port 9999 ..." << std::endl;
                svr.listen("0.0.0.0", 9999);
        }
        catch (const std::exception &e)
        {
                std::cerr << "exception: " << e.what() << std::endl;
        }

        return 1;
}