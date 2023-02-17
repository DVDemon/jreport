#include "confluence_export.h"

#include <map>
#include <iostream>
#include <fstream>

#include "../loader/url_downloader.h"
#include "../config/config.h"
#include "../model/initiative.h"
#include "../model/hrefs.h"

#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <Poco/URI.h>


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
                result = "<td class=\"highlight-#57d9a3\" title=\"Background colour : Medium green 65%\" data-highlight-colour=\"#57d9a3\">";
            if (value == "Отменено")
                result = "<td class=\"highlight-#ff7452\" title=\"Background colour : Medium red 85%\" data-highlight-colour=\"#ff7452\">";
            if (value == "Ревью")
                result = "<td class=\"highlight-#ffc400\" title=\"Background colour : Medium yellow 100%\" data-highlight-colour=\"#ffc400\">";
            if (value == "Обратная связь")
                result = "<td class=\"highlight-#ffc400\" title=\"Background colour : Medium yellow 100%\" data-highlight-colour=\"#ffc400\">";

            result += "<p>" + value + "</p></td>";
            return result;
        };

        content_body += table_header("Cluster");
        content_body += table_header("Product");
        content_body += table_header("Initiative epic");
        content_body += table_header("Product epic");
        content_body += table_header("Assigne");
        content_body += table_header("Status");
        content_body += table_header("Last status change (days)");
        content_body += table_header("Reason");
        content_body += table_header("Links");
        content_body += table_header("Comments");
        content_body += "</tr>";

        struct initiative_body{
            std::string body;
            size_t closed{0};
            size_t progress{0};
            size_t review{0};
            size_t feedback{0};
            size_t reject{0};
            size_t hold{0};
            size_t waiting{0};
        };
        std::map<std::string, initiative_body> initiatives_content;

        for (report::Report &r : report)
        {

            if (initiatives_content.find(r.initative) == std::end(initiatives_content))
                initiatives_content[r.initative] = initiative_body{};
            std::string body;
            body += "<tr>";
            body += table_cell(r.cluster);
            body += table_cell(r.product);

            if (r.issue_status.empty())
            {
                for (size_t i = 0; i < 10; ++i)
                     body += "<td></td>";
            }
            else
            {
                auto [i_status, p_status] = *r.issue_status.begin();
                body += "<td><a href=\"https://jira.mts.ru/browse/" + i_status.key + "\">" + i_status.name + "</a></td>";
                body += "<td><a href=\"https://jira.mts.ru/browse/" + p_status.key + "\">" + p_status.key + "</a></td>";
                body += table_cell(p_status.assigne);
                std::string status = report::Report::map_status(p_status.status);
                body += table_status(status);
                if(status=="Готово") initiatives_content[r.initative].closed++;
                if(status=="В работе")  initiatives_content[r.initative].progress++;
                if(status=="Ревью") initiatives_content[r.initative].review++;
                if(status=="Обратная связь") initiatives_content[r.initative].feedback++;
                if(status=="Приостановлено") initiatives_content[r.initative].hold++;
                if(status=="Отменено") initiatives_content[r.initative].reject++;
                
                size_t change = p_status.status_changed;
                if(change == 0 ) body +=  "<td></td>";
                            else body += table_cell(std::to_string(change));
                body += table_cell(p_status.issue->get_reason());
                body += "<td>";
                for (const auto &[lk,link_type] : p_status.links)
                    body += "<a href=\"https://jira.mts.ru/browse/" + lk + "\">" + link_type +" " + lk + "</a><br/>";

                body += "</td>";
                
                if(p_status.issue->hrefs().empty())
                    body += "<td></td>";
                else{
                    body += "<td>";
                    for (const std::string &hr : p_status.issue->hrefs()){
                        std::string title{hr};
                        if(auto t = model::HREFS::get()[hr])
                          title = *t;
/*                        try{
                            if(hr.find("confluence")){
                                Poco::URI Uri(hr);
                                Poco::URI::QueryParameters qp = Uri.getQueryParameters();
                                std::string  pageId;
                                for(const auto &[n,v] : qp){
                                    if(n=="pageId")
                                        pageId = v;
                                }
                                if(!pageId.empty()){
                                    std::cout << "pageId:" << pageId << std::endl;
                                    auto res = ExportConfluence::get_page(pageId);
                                    if(res){
                                        title = res->second;
                                        std::cout << "title:" << title << std::endl;
                                    }
                                }
                            
                            }
                        }catch(...){}*/
                        body += "<a href=\""+ escape(hr)+"\">"+escape(title)+"</a><br/>";
                    }

                    body += "</td>";
                }
            }
            body += "</tr>";

            initiatives_content[r.initative].body = initiatives_content[r.initative].body + body;
        }
        std::cout << "done" << std::endl;

        for (const auto &[in, ib] : initiatives_content)
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
                    std::optional<std::pair<int,std::string>> result = get_page(page_id);

                    if (result)
                    {
                        auto& [version,title] = *result;

                        std::cout << "put page  ...";
                        Poco::JSON::Object::Ptr data = new Poco::JSON::Object();
                        data->set("id", page_id);
                        data->set("type", "page");
                        data->set("title", title);

                        Poco::JSON::Object::Ptr vobject = new Poco::JSON::Object();
                        vobject->set("number", ++version);
                        data->set("version", vobject);
                        Poco::JSON::Object::Ptr bobject = new Poco::JSON::Object();
                        Poco::JSON::Object::Ptr sobject = new Poco::JSON::Object();

                        sobject->set("representation", "storage");
                        std::string header;
                        header+="<p>";
                        header+="<b>Готово:</b>"+std::to_string(ib.closed)+"&nbsp;";
                        header+="<b>В работе:</b>"+std::to_string(ib.progress)+"&nbsp;";
                        header+="<b>Ревью:</b>"+std::to_string(ib.review)+"&nbsp;";
                        header+="<b>Обратная связь:</b>"+std::to_string(ib.feedback)+"&nbsp;";
                        header+="<b>Приостановлено:</b>"+std::to_string(ib.hold)+"&nbsp;";
                        header+="<b>Отменено:</b>"+std::to_string(ib.reject)+"&nbsp;";
                        header+="</p>";
                        sobject->set("value", content_body + ib.body + "</tbody></table>");

                        // std::ofstream out("content_body.html");
                        // out << content_body;
                        // out.close();

                        bobject->set("storage", sobject);
                        data->set("body", bobject);

                        std::stringstream ss;
                        std::string url = Config::get().get_confluence_address() + "/content/" + page_id;
                        Poco::JSON::Stringifier::stringify(data, ss, 4, -1, Poco::JSON_PRESERVE_KEY_ORDER);

                        if (loaders::Downloader::get().do_put(url, Config::get().get_confluence_identity(), ss.str()))
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
                catch (std::exception &ex)
                {
                    std::cout << "exception: " << ex.what() << std::endl;
                }
            }
        }
    }

        std::optional<std::pair<int,std::string>> ExportConfluence::get_page(const std::string & page_id){
        std::string url = Config::get().get_confluence_address() + "/content/" + page_id;
        return ExportConfluence::get_page_by_url(url,true);
        
    }
    
    std::optional<std::pair<int,std::string>> ExportConfluence::get_page_by_url(const std::string & url,bool log_result){
        //std::cout << "get url:" << url << std::endl;
        std::optional<std::string> result = loaders::Downloader::get().do_get(url, Config::get().get_confluence_identity());

        

        if (result)
            {
                if(log_result) std::cout << *result << std::endl;

                Poco::JSON::Parser parser;
                Poco::Dynamic::Var var = parser.parse(*result);
                Poco::JSON::Object::Ptr json = var.extract<Poco::JSON::Object::Ptr>();
                Poco::JSON::Object::Ptr ver = json->getObject("version");
                int version = ver->getValue<int>("number");
                std::string title = json->getValue<std::string>("title");
                return std::pair<int,std::string>(version,title);
            }
        return std::optional<std::pair<int,std::string>>();
    }
}