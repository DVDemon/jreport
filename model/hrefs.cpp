
#include "../database/database.h"
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include "hrefs.h"

using namespace Poco::Data::Keywords;


    model::HREFS::HREFS()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);

            std::string link;
            std::string title;
            select << "SELECT link,title FROM HREF",
                into(link),
                into(title),
                range(0, 1);

            while (!select.done())
            {
                if (select.execute())
                    _hrefs[link] = title;
            }
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

    model::HREFS & model::HREFS::get()
    {
        static HREFS _hrefs;
        return _hrefs;
    }
    const std::optional<std::string> model::HREFS::operator[](const std::string &key)
    {
        std::optional<std::string> res;
        auto it = _hrefs.find(key);

        if (it != std::end(_hrefs))
            return it->second;

        return res;
    }

    void model::HREFS::set( std::string &link,  std::string &title)
    {
        if (!title.empty())
        {

            _hrefs[link] = title;
            try
            {

                Poco::Data::Session session = database::Database::get().create_session();
                Poco::Data::Statement delete_issue(session);
                std::string l=link;
                std::string sql = database::sql_string("DELETE FROM HREF WHERE link=?",link);
                delete_issue << sql;
                delete_issue.execute();

                Poco::Data::Statement insert_issue(session);

                sql = database::sql_string("INSERT INTO HREF(link,title) VALUES (?,?)",link,title);
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
