#include "model/hrefs.h"
#include "model/product_initiative_issue.h"
#include "model/issue.h"
#include "config/config.h"
#include "file_export/confluence_export.h"

#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/URI.h>

#include <thread>
#include <chrono>

using namespace std::chrono;
auto main() -> int
{

    for (const model::ProductInitativeIssue &pii : model::ProductInitativeIssue::load_all())
    {

        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
        std::time_t t_t = std::chrono::system_clock::to_time_t(now);
        tm local_tm = *localtime(&t_t);
        std::shared_ptr<model::Issue> i = model::Issue::from_mongodb(pii.product_issue, local_tm);
        if (i)
        {
            if (!i->get_hrefs().empty())
            {
                for (std::string link : i->get_hrefs())
                {

                    try
                    {
                        if (link.find("confluence")!= std::string::npos)
                        {
                            
                            std::cout << "   " + link;
                            
                            if (!model::HREFS::get()[link])
                            {
                                std::this_thread::sleep_for(1000ms);
                                Poco::URI Uri(link);
                                Poco::URI::QueryParameters qp = Uri.getQueryParameters();
                                std::string pageId;
                                for (const auto &[n, v] : qp)
                                {
                                    if (n == "pageId")
                                        pageId = v;
                                }
                                if (!pageId.empty())
                                {
                                    std::string url = Config::get().get_confluence_address() + "/content/" + pageId;
                                    auto res = file_export::ExportConfluence::get_page_by_url(url);
                                    if (res)
                                    {
                                        std::string title = res->second;
                                        std::cout << "... title:" << title << std::endl;
                                        model::HREFS::get().set(link, title);
                                    } else std::cout << " ... fail load confluence" << std::endl;
                                } else std::cout << " ... fail get page" << std::endl;
                            } else std::cout << " ... from cache" << std::endl;
                        }
                    }
                    catch (std::exception &ex)
                    {
                        std::cout << "hrefs exception:" << ex.what() << std::endl;
                    }
                }
            }
        }
        
    }
}