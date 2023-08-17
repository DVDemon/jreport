#include "product_project.h"

#include "../database/database.h"
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace model
{
    std::optional<ProductProject> ProductProject::load(const std::string &product)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();

            Statement select(session);
            ProductProject result;
            std::string p = product;

            select << "SELECT product,project FROM Product_Project WHERE product=$1",
                use(p),
                into(result.product),
                into(result.project);

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (!rs.moveFirst())
                return  std::optional<ProductProject>();
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
    ProductProject ProductProject::fromJSON(Poco::JSON::Object::Ptr ptr)
    {
        ProductProject result;

        result.product = ptr->getValue<std::string>("product");
        result.project = ptr->getValue<std::string>("project");

        return result;
    }

    Poco::JSON::Object::Ptr ProductProject::toJSON()
    {
        Poco::JSON::Object::Ptr result = new Poco::JSON::Object();

        result->set("product", product);
        result->set("project", project);

        return result;
    }

    std::vector<ProductProject> ProductProject::load(){

    try
        {
            Poco::Data::Session session = database::Database::get().create_session();

            Statement select(session);
            std::vector<ProductProject> results;
            ProductProject result;

            select << "SELECT product,project FROM Product_Project",
                into(result.product),
                into(result.project),
                range(0,1);

            while(!select.done()){
                if(select.execute()) 
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

    void ProductProject::save()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement delete_issue(session);

            std::string sql = database::sql_string("DELETE FROM Product_Project WHERE product=?", product);
            delete_issue << sql;
            delete_issue.execute();

            Statement insert_issue(session);

            sql = database::sql_string("INSERT INTO Product_Project(product,project) VALUES (?,?)", product,project);
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

}