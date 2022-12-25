#ifndef JIRA_LOADER
#define JIRA_LOADER

#include "../model/issue.h"
#include <chrono>
#include <optional>

//#define STUB
namespace loaders{

    class LoaderJira{
        private:
            LoaderJira();

            std::string load_from_file(const std::string & id);
            std::shared_ptr<model::Issue> parse(const std::string & source);
        public:
            static LoaderJira& get();
            std::shared_ptr<model::Issue> load(const std::string & id,const std::string & identity);
            ~LoaderJira();
    };
}

#endif