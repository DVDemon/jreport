#ifndef XLS_EXPORT
#define XLS_EXPORT

#include "reports.h"
#include <vector>
#include <OpenXLSX/OpenXLSX.hpp>

namespace file_export {
    struct ExportXLS{
        static void export_status(std::vector<report::Report> &report,OpenXLSX::XLDocument& doc);
        static void export_links(std::vector<report::Report> &report,OpenXLSX::XLDocument& doc);
        static void start_export(std::vector<report::Report> &report);
    };

};

#endif