#ifndef MTA_SERVER_HANDLERS_H_
#define MTA_SERVER_HANDLERS_H_

#include "HttpServer.h"

using mta_http_server::HttpServer;

namespace mta_http_server {
    HttpRequestHandler_t handleGetHtml = [](const HttpRequest& request) -> HttpResponse {
        HttpResponse response = HttpResponse(HttpStatusCode::Ok);
        response.SetHeader("Content-Type", "text/html");

        auto it = request.params().find("lng");

        // Default is English
        if (it == request.params().end())
            response.SetContent(string_replace(read_html_file(std::string(".") + to_string(request.uri())), "${FILL_DATA_HERE}", SingletonHtmlPlaceholder::getInstance().get()));

        else {
            std::string file_path;
            // Add try/catch
            QLanguage lang_param = string_to_language(it->second);

            switch (lang_param) {
                case HE:
                    file_path = "./index-he.html";
                    break;
                case FR:
                    file_path = "./index-fr.html";
                    break;
                case EN:
                    file_path = "./index.html";
                default:
                    break;
            }
        }
        
        return response;
    };

    HttpRequestHandler_t handlePutPlaceholder = [](const HttpRequest& request) -> HttpResponse {
        HttpResponse response = HttpResponse(HttpStatusCode::Ok);

        SingletonHtmlPlaceholder::getInstance().set(request.content());

        response.SetHeader("Content-Type", "text/plain");
        response.SetContent("OK");

        return response;
    };

    HttpRequestHandler_t handleRemovePlaceholder = [](const HttpRequest& request) -> HttpResponse {
        HttpResponse response = HttpResponse(HttpStatusCode::Ok);

        SingletonHtmlPlaceholder::getInstance().set("Guest");

        response.SetHeader("Content-Type", "text/plain");
        response.SetContent("OK");

        return response;
    };

    HttpRequestHandler_t handlePostHtml = [](const HttpRequest& request) -> HttpResponse {
        std::cout << request.content() << std::endl;
        return HttpResponse(HttpStatusCode::Ok);
    };

    HttpRequestHandler_t dummy_handler = [](const HttpRequest& request) -> HttpResponse {
        return HttpResponse();
    };

    void RegisterMTAHandlers(HttpServer& server) {
        server.RegisterHttpRequestHandler("/index.html", HttpMethod::POST, handlePostHtml);
        server.RegisterHttpRequestHandler("/index.html", HttpMethod::GET, handleGetHtml);
        server.RegisterHttpRequestHandler("/index.html", HttpMethod::DEL, handleRemovePlaceholder);
        server.RegisterHttpRequestHandler("/index.html", HttpMethod::PUT, handlePutPlaceholder);

        server.RegisterHttpRequestHandler("/index.html", HttpMethod::HEAD, dummy_handler);
        server.RegisterHttpRequestHandler("/index.html", HttpMethod::TRACE, dummy_handler);
        server.RegisterHttpRequestHandler("/index.html", HttpMethod::OPTIONS, dummy_handler);
    }
}
#endif
