#ifndef CONFLUENCE_EXPORT
#define CONFLUENCE_EXPORT

#include "reports.h"
#include <vector>


namespace file_export {
    struct ExportConfluence{
        static std::optional<std::pair<int,std::string>> get_page_by_url(const std::string & url,bool log_result=false);
        static std::optional<std::pair<int,std::string>> get_page(const std::string & page_id);
        static void start_export(std::vector<report::Report> &report);
    };

};

#endif