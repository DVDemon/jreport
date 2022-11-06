#ifndef XLS_EXPORT
#define XLS_EXPORT

#include "reports.h"
#include <vector>

namespace file_export {
    struct ExportXLS{
        static void start_export(std::vector<report::Report> &report);
    };

};

#endif