#include "cluster_project.h"

#include "../database/database.h"
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

#include <map>
#include <set>

#include "product_project.h"
#include "product.h"

namespace model
{
    std::optional<ClusterProject> ClusterProject::load(const std::string &project)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();

            Statement select(session);
            ClusterProject result;
            std::string p = project;

            select << "SELECT cluster,project FROM Cluster_Project WHERE project=?",
                use(p),
                into(result.cluster),
                into(result.project);

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (!rs.moveFirst())
                return  std::optional<ClusterProject>();
            return result;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }
    ClusterProject ClusterProject::fromJSON(Poco::JSON::Object::Ptr ptr)
    {
        ClusterProject result;

        result.cluster = ptr->getValue<std::string>("cluster");
        result.project = ptr->getValue<std::string>("project");

        return result;
    }

    Poco::JSON::Object::Ptr ClusterProject::toJSON()
    {
        Poco::JSON::Object::Ptr result = new Poco::JSON::Object();

        result->set("cluster", cluster);
        result->set("project", project);

        return result;
    }



    void ClusterProject::fill(){
         std::vector<model::ProductProject> array = model::ProductProject::load();

         std::map<std::string,std::set<std::string>> clusters;

         for(model::ProductProject &pp : array){
            if(Products::get().products().find(pp.product)!=std::end(Products::get().products())){
                std::shared_ptr<model::Product> prod_ptr = model::Products::get().products()[pp.product];
                if(prod_ptr){
                    std::cout << prod_ptr->cluster << std::endl;
                    if(clusters.find(prod_ptr->cluster)==std::end(clusters)){
                        clusters[prod_ptr->cluster] = std::set<std::string>();
                    }
                    clusters[prod_ptr->cluster].insert(pp.project);
                }
            } 
         }
        
         for(auto & [c,p] : clusters){
                for(auto & s: p){
                    ClusterProject cp;
                    cp.cluster = c;
                    cp.project = s;
                    cp.save();
                }
            }
    }

    void ClusterProject::save()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement delete_issue(session);

            delete_issue << "DELETE FROM Cluster_Project WHERE project=? ",
                use(project); 
            delete_issue.execute();

            Statement insert_issue(session);

            insert_issue << "INSERT INTO Cluster_Project(cluster,project) VALUES (?,?)",
                use(cluster),
                use(project);
            insert_issue.execute();
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

}