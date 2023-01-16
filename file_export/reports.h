#ifndef REPORTS_H
#define REPORTS_H

#include <string>
#include <vector>
#include <utility>

#include "../model/issue.h"

namespace report{
    struct Report_Issue{
        std::string key;
        std::string name;
        std::string status;
        std::string assigne;
        size_t      day_shift;
        size_t      status_changed;
        std::shared_ptr<model::Issue> issue;
        std::vector<std::string> links;
    };

    struct Report{
        std::string initative;
        std::string cluster;
        std::string product;

        static std::string map_status(const std::string & old_status);

        std::vector<std::pair<Report_Issue,Report_Issue>> issue_status;
    };
}

#endif