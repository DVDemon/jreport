#ifndef CONFLUENCE_EXPORT
#define CONFLUENCE_EXPORT

#include "reports.h"
#include <vector>


namespace file_export {
    struct ExportConfluence{
        static void start_export(std::vector<report::Report> &report);
    };

};

#endif