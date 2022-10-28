#include "jira_loader.h"

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

#include <memory>

//Authorization=ZHZkenl1YjE6My4xNE9uZWVyMg==
namespace loaders
{
    LoaderJira::LoaderJira()
    {
        Poco::Net::initializeSSL();
        Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> ptrCert = new Poco::Net::AcceptCertificateHandler(false);
        Poco::Net::Context::Ptr ptrContext = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", "rootcert.pem", Poco::Net::Context::VERIFY_STRICT, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
        Poco::Net::SSLManager::instance().initializeClient(0, ptrCert, ptrContext);
    }

    LoaderJira &LoaderJira::get()
    {
        static LoaderJira _instance;
        return _instance;
    }
    std::optional<model::Issue> LoaderJira::load([[maybe_unused]] const std::string &id)
    {
        std::optional<model::Issue> result;

        try
        {
            Poco::URI uri("https://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&hourly=temperature_2m,relativehumidity_2m,windspeed_10m");
            Poco::Net::HTTPSClientSession s(uri.getHost(), uri.getPort());
            Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri.toString());
            request.set("user-agent", "Poco HTTPSClientSession");
            s.sendRequest(request);
            Poco::Net::HTTPResponse response;
            std::istream &rs = s.receiveResponse(response);
            Poco::StreamCopier::copyStream(rs, std::cout);
        }
        catch (Poco::Exception &ex)
        {
            std::cout << ex.displayText() << std::endl;
        }
        return result;
    }

    LoaderJira::~LoaderJira(){
        Poco::Net::uninitializeSSL();
    }
}