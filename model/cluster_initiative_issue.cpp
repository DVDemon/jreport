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

    std::optional<ClusterInitativeIssue> ClusterInitativeIssue::load(const std::string &cluster, const std::string &initiative,const std::string &initiative_issue){

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();

            Statement select(session);
            ClusterInitativeIssue result;

            std::string c = cluster;
            std::string ii = initiative_issue;
            std::string i = initiative;

            select << "SELECT issue,initiative_issue,initiative,cluster FROM Cluster_Initiative_Issue WHERE cluster=? AND initiative_issue=? AND initiative=?",
                use(c),
                use(ii),
                use(i),
                into(result.issue),
                into(result.initiative_issue),
                into(result.initiative),
                into(result.cluster),
                range(0,1);

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (!rs.moveFirst()) return std::optional<ClusterInitativeIssue>();
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

    void ClusterInitativeIssue::save(){
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement delete_issue(session);

            delete_issue << "DELETE FROM Cluster_Initiative_Issue WHERE issue=?",
                use(issue); //  iterate over result set one row at a time
            delete_issue.execute();

            Statement insert_issue(session);

            insert_issue << "INSERT INTO Cluster_Initiative_Issue(issue,initiative_issue,initiative,cluster) VALUES (?,?,?,?)",
                use(issue),
                use(initiative_issue),
                use(initiative),
                use(cluster);
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
} // namespace model
