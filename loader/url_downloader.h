#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <optional>
#include <string>

namespace loaders{
    class Downloader{
        private:
            Downloader();
        public:
            static Downloader& get();
            std::optional<std::string> do_get(const std::string& url,const std::string &identity);
            ~Downloader();
    };
}
#endif
