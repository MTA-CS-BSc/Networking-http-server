#ifndef MTA_SERVER_HANDLERS_H_
#define MTA_SERVER_HANDLERS_H_

#include "HttpServer.h"

using mta_http_server::HttpServer;

namespace mta_http_server {
    HttpRequestHandler_t handleGetHtml = [](const HttpRequest& request) -> HttpResponse {
        HttpResponse response = HttpResponse(HttpStatusCode::Ok);
        response.SetHeader("Content-Type", "text/html");

        std::string file_path, line_to_insert;
        QLanguage lang_param = QLanguage::EN;

        auto it = request.params().find("lng");

        if (it != request.params().end())
            lang_param = string_to_language(it->second); // Add try/catch

        std::string placeholder = SingletonHtmlPlaceholder::getInstance().get();

        switch (lang_param) {
        case QLanguage::HE:
            file_path = "./index-he.html";
            if (placeholder.empty())
                line_to_insert = "הממ... לא נראה שיש כאן משהו";
            else
                line_to_insert = "מישהו מוסר: " + placeholder;
            break;
        case QLanguage::FR:
            file_path = "./index-fr.html";
            if (placeholder.empty())
                line_to_insert = "Il ne semble pas y avoir quelque chose ici...";
            else
                line_to_insert = "Quelqu'un a laissé un message: " + placeholder;
            break;
        case QLanguage::EN:
        default:
            file_path = "./index.html";
            if (placeholder.empty())
                line_to_insert = "Hmm... It doesn't look like there's something here";
            else
                line_to_insert = "Someone left a message: " + placeholder;
            break;
        }
        
        response.SetContent(string_replace(read_html_file(file_path), "${PLACEHOLDER}", line_to_insert));
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
