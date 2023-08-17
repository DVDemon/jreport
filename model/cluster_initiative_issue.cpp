#include "cluster_initiative_issue.h"

#include "../database/database.h"
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace model
{
    
    ClusterInitativeIssue ClusterInitativeIssue::fromJSON(Poco::JSON::Object::Ptr ptr){
        ClusterInitativeIssue result;

        result.cluster = ptr->getValue<std::string>("cluster");
        result.initiative = ptr->getValue<std::string>("initiative");
        result.initiative_issue = ptr->getValue<std::string>("initiative_issue");
        result.issue = ptr->getValue<std::string>("issue");

        return result;
    }

    std::optional<ClusterInitativeIssue> ClusterInitativeIssue::load(const std::string &cluster, const std::string &initiative_issue){

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();

            Statement select(session);
            ClusterInitativeIssue result;

            std::string sql = database::sql_string("SELECT issue,initiative_issue,initiative,cluster FROM Cluster_Initiative_Issue WHERE cluster=? AND initiative_issue=?", cluster, initiative_issue);

            select << sql;
            select.execute();

            Poco::Data::RecordSet rs(select);
            if (rs.rowCount())
                for (auto &row : rs)
                {

                    result.issue = row["issue"].toString();
                    result.initiative_issue = row["initiative_issue"].toString();
                    result.initiative = row["initiative"].toString();
                    result.cluster = row["cluster"].toString();
                }
            else
                return std::optional<ClusterInitativeIssue>();
            
            return result;
        }

        catch (Poco::Data::PostgreSQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::PostgreSQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    void ClusterInitativeIssue::save(){
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement delete_issue(session);

            delete_issue << "DELETE FROM Cluster_Initiative_Issue WHERE initiative=$1 and cluster=$2 and initiative_issue=$3",
                use(initiative),
                use(cluster),
                use(initiative_issue); //  iterate over result set one row at a time
            delete_issue.execute();

            Statement insert_issue(session);

            insert_issue << "INSERT INTO Cluster_Initiative_Issue(issue,initiative_issue,initiative,cluster) VALUES ($1,$2,$3,$4)",
                use(issue),
                use(initiative_issue),
                use(initiative),
                use(cluster);
            insert_issue.execute();
        }

        catch (Poco::Data::PostgreSQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::PostgreSQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }

    }
} // namespace model
