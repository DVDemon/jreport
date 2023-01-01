#include "url_downloader.h"

#include "Poco/StreamCopier.h"
#include "Poco/URI.h"
#include "Poco/Exception.h"
#include "Poco/SharedPtr.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/Net/KeyConsoleHandler.h"
#include "Poco/Net/ConsoleCertificateHandler.h"
#include "Poco/Net/AcceptCertificateHandler.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <Poco/URIStreamFactory.h>
#include <Poco/URIStreamOpener.h>
#include <Poco/Net/HTTPSStreamFactory.h>
#include <Poco/Net/HTTPStreamFactory.h>
#include <Poco/Base64Encoder.h>

#include "../config/config.h"

namespace loaders
{

    Downloader::Downloader()
    {
        Poco::Net::initializeSSL();
        Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> ptrCert = new Poco::Net::AcceptCertificateHandler(false);
        Poco::Net::Context::Ptr ptrContext = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "");
        Poco::Net::SSLManager::instance().initializeClient(0, ptrCert, ptrContext);
    }

    Downloader &Downloader::get()
    {
        static Downloader _instance;
        return _instance;
    }
    std::optional<std::string> Downloader::do_get(const std::string &url, const std::string &identity)
    {
        std::string string_result;
        try
        {
            Poco::URI uri(url);
            Poco::Net::HTTPSClientSession s(uri.getHost(), uri.getPort());
            Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri.toString());
            request.setVersion(Poco::Net::HTTPMessage::HTTP_1_1);
            request.setContentType("application/json");
            request.set("Authorization", identity);
            request.set("Accept", "application/json");
            request.setKeepAlive(true);

            s.sendRequest(request);
  
            Poco::Net::HTTPResponse response;
            std::istream &rs = s.receiveResponse(response);

            while (rs)
            {
                char c{};
                rs.read(&c, 1);
                if (rs)
                    string_result += c;
            }
        }
        catch (Poco::Exception &ex)
        {
            return std::optional<std::string>();
        }

        return string_result;
    }

    std::optional<std::string> Downloader::do_put(const std::string& url,const std::string &identity,const std::string &data){
         std::string string_result;
        try
        {
            Poco::URI uri(url);
            Poco::Net::HTTPSClientSession s(uri.getHost(), uri.getPort());
            Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_PUT, uri.toString());
            request.setVersion(Poco::Net::HTTPMessage::HTTP_1_1);
            request.setContentType("application/json");
            request.set("Authorization", identity);
            request.set("Accept", "application/json");
            request.set("User-Agent","jreport application");
            request.setContentLength(data.size());
            request.setKeepAlive(true);


            std::ostream& o = s.sendRequest(request);
            o << data;
            o.flush();
  
            Poco::Net::HTTPResponse response;
            std::istream &rs = s.receiveResponse(response);

            std::cout << response.getStatus() << std::endl;
            while (rs)
            {
                char c{};
                rs.read(&c, 1);
                if (rs)
                    string_result += c;
            }
           // std::cout <<"[" << string_result << "]" << std::endl;
        }
        catch (Poco::Exception &ex)
        {
            return std::optional<std::string>();
        }

        return string_result;
    }

    Downloader::~Downloader(){
        Poco::Net::uninitializeSSL();
    }
}