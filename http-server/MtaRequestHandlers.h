#ifndef MTA_SERVER_HANDLERS_H_
#define MTA_SERVER_HANDLERS_H_

#include "HttpServer.h"
#include "FilePaths.h"

#define LANGUAGE_PARAM_KEY "lang"

using mta_http_server::HttpServer;

namespace mta_http_server {
    HttpRequestHandler_t handleGetHtml = [](const HttpRequest& request) -> HttpResponse {
        HttpResponse response = HttpResponse(HttpStatusCode::Ok);
        response.SetHeader("Content-Type", "text/html");

        std::string file_path, line_to_insert;
        int last_line_length;
        QLanguage lang_param = QLanguage::EN;

        auto it = request.params().find(LANGUAGE_PARAM_KEY);

        if (it != request.params().end()) {
            try {
                lang_param = string_to_language(it->second);
            } catch {
                lang_param = QLanguage::EN;
            }
        }

        std::string placeholder = SingletonHtmlPlaceholder::getInstance().get();

        if (!placeholder.empty())
            line_to_insert = placeholder;

        switch (lang_param) {
        case QLanguage::HE:
            file_path = INDEX_HTML_HE_PATH;
            last_line_length = 32;
            break;
        case QLanguage::FR:
            file_path = INDEX_HTML_FR_PATH;
            last_line_length = 27;
            break;
        case QLanguage::EN:
        default:
            file_path = INDEX_HTML_EN_PATH;
            last_line_length = 24;
            break;
        }
        
        std::string altered_html = string_replace(read_html_file(file_path), "${PLACEHOLDER}", line_to_insert);

        if (line_to_insert.empty())
            altered_html = altered_html.erase(altered_html.find("<h4>"), last_line_length);
            
        response.SetContent(altered_html);
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

        SingletonHtmlPlaceholder::getInstance().set("");

        response.SetHeader("Content-Type", "text/plain");
        response.SetContent("OK");

        return response;
    };

    HttpRequestHandler_t handlePostHtml = [](const HttpRequest& request) -> HttpResponse {
        std::cout << request.content() << std::endl;
        return HttpResponse(HttpStatusCode::Ok);
    };

    HttpRequestHandler_t dummy_handler = [](const HttpRequest& request) -> HttpResponse {
        return HttpResponse(HttpStatusCode::Ok);
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
