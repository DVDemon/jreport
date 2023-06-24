#include "comment.h"

#include "../database/database.h"
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace model{
std::optional<Comment> Comment::load(const std::string &product, const  std::string &cluster_issue){
 try
        {
            Poco::Data::Session session = database::Database::get().create_session();

            Statement select(session);
            Comment result;
            std::string p = product;
            std::string ci = cluster_issue;

            select << "SELECT product,cluster_issue,comment,address FROM Product_Initiative_Comment WHERE product=? AND cluster_issue=?",
                use(p),
                use(ci),
                into(result.product),
                into(result.cluster_issue),
                into(result.comment),
                into(result.address);

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (!rs.moveFirst())
                return std::optional<Comment>();
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
Comment Comment::fromJSON(Poco::JSON::Object::Ptr ptr){
    Comment result;

        result.product = ptr->getValue<std::string>("product");
        result.cluster_issue = ptr->getValue<std::string>("cluster_issue");
        result.comment = ptr->getValue<std::string>("comment");
        result.address = ptr->getValue<std::string>("address");

        return result;
}

Poco::JSON::Object::Ptr Comment::toJSON(){
    Poco::JSON::Object::Ptr result = new Poco::JSON::Object();

    result->set("product",product);
    result->set("cluster_issue",cluster_issue);
    result->set("comment",comment);
    result->set("address",address);
    
    return result;
}   

void Comment::save(){
    try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement delete_issue(session);

            delete_issue << "DELETE FROM Product_Initiative_Comment WHERE product=? AND cluster_issue=?",
                use(product),
                use(cluster_issue); //  iterate over result set one row at a time
            delete_issue.execute();

            Statement insert_issue(session);

            insert_issue << "INSERT INTO Product_Initiative_Comment(product,cluster_issue,comment,address) VALUES (?,?,?,?)",
                use(product),
                use(cluster_issue),
                use(comment),
                use(address);
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