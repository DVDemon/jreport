#ifndef JIRA_LOADER
#define JIRA_LOADER

#include "../model/issue.h"
#include <optional>
namespace loaders{
    struct LoaderJira{
        static std::optional<model::Issue> load(const std::string & id);
    };
}

#endif