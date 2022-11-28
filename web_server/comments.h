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

#include "../model/comment.h"

class Comments : public HTTPRequestHandler
{
private:
public:
    Comments(const std::string &format) : _format(format) {}

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
        std::cout << "comment" << std::endl;
        try
        {
            if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST)
            {
                auto &stream = request.stream();
                const size_t len = request.getContentLength();
                std::string buffer(len, 0);
                stream.read(buffer.data(), len);

                Poco::JSON::Parser parser;
                Poco::Dynamic::Var var = parser.parse(buffer);
                Poco::JSON::Object::Ptr json = var.extract<Poco::JSON::Object::Ptr>();
                model::Comment com = model::Comment::fromJSON(json);
                com.save();
                response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                return;
            }

            HTMLForm form(request, request.stream());
            if (form.has("product") &&
                form.has("cluster_issue"))
            {

                std::string product = form.get("product").c_str();
                std::string cluster_issue = form.get("cluster_issue").c_str();

                std::cout << "product:" << product << ", issue:" << cluster_issue << std::endl;

                model::Comment com = model::Comment::load(product, cluster_issue);
                
                response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                auto &ss = response.send();
                Poco::JSON::Stringifier::stringify(com.toJSON(), ss, 4, -1, Poco::JSON_PRESERVE_KEY_ORDER);
                ss.flush();
                
            }
            return;
        }
        catch (...)
        {

            return;
        }

        response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
        response.send();
    }

private:
    std::string _format;
};
