#pragma once

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/JSON/Parser.h"
#include <iostream>
#include <iostream>
#include <fstream>

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTMLForm;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::NameValueCollection;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#include "../model/product_initiative_issue.h"

class ProductInitiativeIssue : public HTTPRequestHandler
{
public:
    ProductInitiativeIssue(const std::string &format) : _format(format) {}

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {

        try
        {
            if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST)
            {
                std::cout << "set product initiative issue" << std::endl;
                auto &stream = request.stream();
                const size_t len = request.getContentLength();
                std::string buffer(len, 0);
                stream.read(buffer.data(), len);

                Poco::JSON::Parser parser;
                Poco::Dynamic::Var var = parser.parse(buffer);
                Poco::JSON::Object::Ptr json = var.extract<Poco::JSON::Object::Ptr>();
                model::ProductInitativeIssue cii = model::ProductInitativeIssue::fromJSON(json);
                cii.save();
                response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                return;
            }
            HTMLForm form(request, request.stream());
            if (form.has("product_issue") &&
                form.has("cluster_issue"))
            {
                std::cout << "get product initiative issue" << std::endl;
                std::string product_issue = form.get("product_issue").c_str();
                std::string cluster_issue = form.get("cluster_issue").c_str();
                std::optional<model::ProductInitativeIssue> pii = model::ProductInitativeIssue::load_by_issue(product_issue, cluster_issue);

                if (pii)
                {
                    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    auto &ss = response.send();
                    std::string str;
                    str = "{ \"issue\" : \"";
                    str += pii->product;
                    str += "\"}";
                    ss << str;
                    ss.flush();
                }
                else
                {
                    response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                    response.send();
                    return;
                }
            }
        }
        catch (...)
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
            response.send();
            return;
        }

        response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
        response.send();
    }

private:
    std::string _format;
};
