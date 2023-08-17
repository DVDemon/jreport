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

    std::optional<ProductInitativeIssue> ProductInitativeIssue::load_by_issue(std::string &product_issue, std::string &cluster_issue)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();

            Statement select(session);
            ProductInitativeIssue result;
            std::string sql = database::sql_string("SELECT product,cluster_issue,product_issue FROM Product_Initiative_Issue WHERE product_issue=? AND cluster_issue=?", product_issue, cluster_issue);

            select << sql;
            select.execute();

            Poco::Data::RecordSet rs(select);
            if (rs.rowCount())
                for (auto &row : rs)
                {
                    result.product = row["product"].toString();
                    result.product_issue = row["product_issue"].toString();
                    result.cluster_issue = row["cluster_issue"].toString();
                }
            else
                return std::optional<ProductInitativeIssue>();

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

    std::vector<ProductInitativeIssue> ProductInitativeIssue::load_by_cluster_issue(std::string &cluster_issue)
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();

            Statement select(session);
            std::vector<ProductInitativeIssue> results;

            std::string sql = database::sql_string("SELECT product,cluster_issue,product_issue FROM Product_Initiative_Issue WHERE cluster_issue=?", cluster_issue);

            select << sql;
            select.execute();

            Poco::Data::RecordSet rs(select);

            for (auto &row : rs)
            {
                ProductInitativeIssue result;
                result.product = row["product"].toString();
                result.product_issue = row["product_issue"].toString();
                result.cluster_issue = row["cluster_issue"].toString();
                results.push_back(result);
            }

            return results;
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

    std::vector<ProductInitativeIssue> ProductInitativeIssue::load_all()
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();

            Statement select(session);
            std::vector<ProductInitativeIssue> results;
            ProductInitativeIssue result;

            std::string sql = "SELECT product,cluster_issue,product_issue FROM Product_Initiative_Issue";
            select << sql;

            select.execute();

            Poco::Data::RecordSet rs(select);
            if (rs.rowCount())
                for (auto &row : rs)
                {
                    result.product = row["product"].toString();
                    result.cluster_issue = row["cluster_issue"].toString();
                    result.product_issue = row["product_issue"].toString();
                    results.push_back(result);
                }


            return results;
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

    ProductInitativeIssue ProductInitativeIssue::load(const std::string &product, const std::string &cluster_issue)
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();

            Statement select(session);
            ProductInitativeIssue result;
            std::string p = product;
            std::string ci = cluster_issue;

            select << "SELECT product,cluster_issue,product_issue FROM Product_Initiative_Issue WHERE product=$1 AND cluster_issue=$2",
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

    void ProductInitativeIssue::save()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement delete_issue(session);

            std::string sql = database::sql_string("DELETE FROM Product_Initiative_Issue WHERE product_issue=?", product_issue);

            delete_issue << sql; //  iterate over result set one row at a time
            delete_issue.execute();

            Statement insert_issue(session);

            sql = database::sql_string("INSERT INTO Product_Initiative_Issue(product,cluster_issue,product_issue) VALUES (?,?,?)", product,cluster_issue,product_issue);
            insert_issue << sql;
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