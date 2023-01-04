#include "confluence_export.h"

#include <map>
#include <iostream>
#include <fstream>

#include "../loader/url_downloader.h"
#include "../config/config.h"
#include "../model/initiative.h"

#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

namespace file_export
{
    void ExportConfluence::start_export([[maybe_unused]] std::vector<report::Report> &report)
    {

        std::cout << "creating content ...";
        std::string content_body;

        content_body = "<table><tbody><tr>";
        auto escape = [&](const std::string &data) -> std::string
        {
            std::string buffer;
            buffer.reserve(data.size());
            for (size_t pos = 0; pos != data.size(); ++pos)
            {
                switch (data[pos])
                {
                case '&':
                    buffer.append("&amp;");
                    break;
                case '\"':
                    buffer.append("&quot;");
                    break;
                case '\'':
                    buffer.append("&apos;");
                    break;
                case '<':
                    buffer.append("&lt;");
                    break;
                case '>':
                    buffer.append("&gt;");
                    break;
                default:
                    buffer.append(&data[pos], 1);
                    break;
                }
            }
            return buffer;
        };

        auto table_header = [&](const std::string &value)
        {
            return "<th><p><strong>" + value + "</strong></p></th>";
        };
        auto table_cell = [&](const std::string &value)
        { return "<td><p>" + escape(value) + "</p></td>"; };

        auto table_status = [&](const std::string &value)
        {
            std::string result = "<td>";

            if (value == "Готово")
                result = "<td data-highlight-colour=\"#e3fcef\">";
            if (value == "Отменено")
                result = "<td data-highlight-colour=\"#fce3ef\">";
            if (value == "Ревью")
                result = "<td data-highlight-colour=\"#ffe400\">";
            if (value == "Обратная связь")
                result = "<td data-highlight-colour=\"#ffe400\">";

            result += "<p>" + value + "</p></td>";
            return result;
        };

        content_body += table_header("Cluster");
        content_body += table_header("Product");
        content_body += table_header("Epic");
        content_body += table_header("Epic name");
        content_body += table_header("Epic link");
        content_body += table_header("Assigne");
        content_body += table_header("Status");
        content_body += table_header("Last status change");
        content_body += table_header("Links");
        content_body += table_header("Comments");
        content_body += "</tr>";

        std::map<std::string, std::string> initiatives_content;

        for (report::Report &r : report)
        {

            if (initiatives_content.find(r.initative) == std::end(initiatives_content))
                initiatives_content[r.initative] = "";
            std::string body;
            body += "<tr>";
            body += table_cell(r.cluster);
            body += table_cell(r.product);

            if (r.issue_status.empty())
            {
                for (size_t i = 0; i < 8; ++i)
                    content_body += "<td></td>";
            }
            else
            {
                auto [i_status, p_status] = *r.issue_status.begin();
                body += table_cell(i_status.key);
                body += table_cell(i_status.name);
                body += "<td><a href=\"https://jira.mts.ru/browse/" + p_status.key + "\">" + p_status.key + "</a></td>";
                body += table_cell(p_status.assigne);
                body += table_status(report::Report::map_status(p_status.status));
                for (size_t i = 0; i < 3; ++i)
                    content_body += "<td></td>";
            }
            body += "</tr>";
            initiatives_content[r.initative] = initiatives_content[r.initative] + body;
        }
        std::cout << "done" << std::endl;

        for (const auto &[in, body] : initiatives_content)
        {
            //
            std::shared_ptr<model::Initiative> ptr;
            for (auto p : model::Initiatives::get().initiatives())
            {
                if (p->name == in)
                    ptr = p;
            }

            if (ptr)
            {
                std::string page_id = ptr->confluence_page;
                std::cout << "get page version id " << page_id << " ...";

                try
                {
                    std::string url = Config::get().get_confluence_address() + "/content/" + page_id;
                    std::cout << url << std::endl;
                    std::optional<std::string> result = loaders::Downloader::get().do_get(url, Config::get().get_confluence_identity());

                    if (result)
                    {
                        Poco::JSON::Parser parser;
                        Poco::Dynamic::Var var = parser.parse(*result);
                        Poco::JSON::Object::Ptr json = var.extract<Poco::JSON::Object::Ptr>();
                        Poco::JSON::Object::Ptr ver = json->getObject("version");
                        int version = ver->getValue<int>("number");
                        std::cout << version << std::endl;

                        std::cout << "put page  ...";
                        Poco::JSON::Object::Ptr data = new Poco::JSON::Object();
                        data->set("id", page_id);
                        data->set("type", "page");
                        data->set("title", "new page");

                        Poco::JSON::Object::Ptr vobject = new Poco::JSON::Object();
                        vobject->set("number", ++version);
                        data->set("version", vobject);
                        Poco::JSON::Object::Ptr bobject = new Poco::JSON::Object();
                        Poco::JSON::Object::Ptr sobject = new Poco::JSON::Object();

                        sobject->set("representation", "storage");
                        sobject->set("value", content_body + body + "</tbody></table>");

                        // std::ofstream out("content_body.html");
                        // out << content_body;
                        // out.close();

                        bobject->set("storage", sobject);
                        data->set("body", bobject);

                        std::stringstream ss;
                        Poco::JSON::Stringifier::stringify(data, ss, 4, -1, Poco::JSON_PRESERVE_KEY_ORDER);

                        result = loaders::Downloader::get().do_put(url, Config::get().get_confluence_identity(), ss.str());
                        if (result)
                        {
                            std::cout << "done" << std::endl;
                        }
                        else
                        {
                            std::cout << "fail" << std::endl;
                        }
                    }
                    else
                        std::cout << "fail" << std::endl;
                }
                catch (...)
                {
                    std::cout << "exception" << std::endl;
                }
            }
        }
    }
}