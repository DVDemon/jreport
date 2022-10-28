#ifndef JIRA_LOADER
#define JIRA_LOADER

#include "../model/issue.h"
#include <optional>
namespace loaders{
    class LoaderJira{
        private:
            LoaderJira();
        public:
            static LoaderJira& get();
            std::optional<model::Issue> load(const std::string & id);

            ~LoaderJira();
    };
}

#endif