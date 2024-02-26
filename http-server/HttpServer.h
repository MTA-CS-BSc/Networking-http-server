#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include <functional>
#include "HttpMessage.h"
#include "Uri.h"

using mta_http_server::HttpRequest;
using mta_http_server::HttpMethod;

namespace mta_http_server {
    // A request handler should expect a request as argument and returns a response
    using HttpRequestHandler_t = std::function<HttpResponse(const HttpRequest&)>;

	class HttpServer {
    private:
        std::string host_;
        std::uint16_t port_;
        bool running_;

    public:
        HttpServer() = default;
        explicit HttpServer(const std::string& host, std::uint16_t port) : host_(host), port_(port) { }
        HttpServer(HttpServer&&) = default;
        HttpServer& operator=(HttpServer&&) = default;
        ~HttpServer() = default;

        void Start();
        void Listen();
        void Stop();

        std::string host() const { return host_; }
        std::uint16_t port() const { return port_; }
        bool running() const { return running_; }
    };
}
#endif //HTTP_SERVER_H