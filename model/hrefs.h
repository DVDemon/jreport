#ifndef H_HREF
#define H_HREF

#include <string>
#include <map>
#include <optional>

namespace model
{
    class HREFS
    {
    private:
        std::map<std::string, std::string> _hrefs;
        HREFS();

    public:
        static HREFS &get();
        const std::optional<std::string> operator[](const std::string&);
        void set( std::string&, std::string&);
    };
}
#endif