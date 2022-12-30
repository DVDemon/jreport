#ifndef REPORTS_H
#define REPORTS_H

#include <string>
#include <vector>
#include <utility>

namespace report{
    struct Report_Issue{
        std::string key;
        std::string name;
        std::string status;
        std::string assigne;
        size_t      day_shift;
        std::vector<std::string> links;
    };

    struct Report{
        std::string initative;
        std::string cluster;
        std::string product;

        std::vector<std::pair<Report_Issue,Report_Issue>> issue_status;
    };
}

#endif