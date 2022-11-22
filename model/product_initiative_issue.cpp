#include "product_initiative_issue.h"

#include "../database/database.h"
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace model
{

    ProductInitativeIssue ProductInitativeIssue::fromJSON(Poco::JSON::Object::Ptr ptr)
    {
        ProductInitativeIssue result;

        result.product = ptr->getValue<std::string>("product");
        result.product_issue = ptr->getValue<std::string>("product_issue");
        result.cluster_issue = ptr->getValue<std::string>("cluster_issue");

        return result;
    }

    ProductInitativeIssue ProductInitativeIssue::load_by_issue(std::string &product_issue, std::string &cluster_issue)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();

            Statement select(session);
            ProductInitativeIssue result;

            select << "SELECT product,cluster_issue,product_issue FROM Product_Initiative_Issue WHERE product_issue=? AND cluster_issue=?",
                use(product_issue),
                use(cluster_issue),
                into(result.product),
                into(result.cluster_issue),
                into(result.product_issue);

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (!rs.moveFirst())
                throw std::logic_error("not found");
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

    std::vector<ProductInitativeIssue> ProductInitativeIssue::load_by_cluster_issue(std::string &cluster_issue){

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();

            Statement select(session);
            std::vector<ProductInitativeIssue> results;
            ProductInitativeIssue result;
            std::string ci = cluster_issue;

            select << "SELECT product,cluster_issue,product_issue FROM Product_Initiative_Issue WHERE cluster_issue=?",
                use(ci),
                into(result.product),
                into(result.cluster_issue),
                into(result.product_issue);

            while(!select.done()){
                if(select.execute()) results.push_back(result);
            }

            return results;
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
    ProductInitativeIssue ProductInitativeIssue::load(const std::string &product, const std::string &cluster_issue)
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();

            Statement select(session);
            ProductInitativeIssue result;
            std::string p = product;
            std::string ci = cluster_issue;

            select << "SELECT product,cluster_issue,product_issue FROM Product_Initiative_Issue WHERE product=? AND cluster_issue=?",
                use(p),
                use(ci),
                into(result.product),
                into(result.cluster_issue),
                into(result.product_issue);

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (!rs.moveFirst())
                throw std::logic_error("not found");
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

    void ProductInitativeIssue::save()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement delete_issue(session);

            delete_issue << "DELETE FROM Product_Initiative_Issue WHERE product_issue=?",
                use(product_issue); //  iterate over result set one row at a time
            delete_issue.execute();

            Statement insert_issue(session);

            insert_issue << "INSERT INTO Product_Initiative_Issue(product,cluster_issue,product_issue) VALUES (?,?,?)",
                use(product),
                use(cluster_issue),
                use(product_issue);
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