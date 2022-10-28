#ifndef JIRA_LOADER
#define JIRA_LOADER

#include "../model/issue.h"
#include <optional>

#define STUB
namespace loaders{

    class LoaderJira{
        private:
            LoaderJira();

            std::string load_from_file(const std::string & id);
        public:
            static LoaderJira& get();
            std::optional<model::Issue> load(const std::string & id);

            ~LoaderJira();
    };
}

#endif