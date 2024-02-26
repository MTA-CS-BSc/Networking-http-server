#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include <functional>
#include "HttpMessage.h"
#include "Settings.h"
#include "Uri.h"
#include <stdexcept>
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include <vector>

using mta_http_server::HttpRequest;
using mta_http_server::HttpMethod;

typedef struct socket_state_t {
    SOCKET id;			// Socket handle
    int	recv;			// Receiving?
    int	send;			// Sending?
    int send_sub_type;	// Sending sub-type
    char buffer[Settings::MAX_BUFFER_SIZE];
    int len;
} SOCKET_STATE;

namespace mta_http_server {
    // A request handler should expect a request as argument and returns a response
    using HttpRequestHandler_t = std::function<HttpResponse(const HttpRequest&)>;

	class HttpServer {
    private:
        std::string host_;
        std::uint16_t port_;
        bool running_;
        std::map<Uri, std::map<HttpMethod, HttpRequestHandler_t>> request_handlers_;
        sockaddr_in server_service_;
        std::vector<SOCKET_STATE> sockets_;

    public:
        HttpServer() = default;
        explicit HttpServer(const std::string& host, std::uint16_t port) : 
            host_(host), port_(port), running_(false),
            request_handlers_(), server_service_(), sockets_(std::vector<SOCKET_STATE>()) { }
        HttpServer(HttpServer&&) = default;
        HttpServer& operator=(HttpServer&&) = default;
        ~HttpServer() = default;

        void Start();
        void Listen(SOCKET& listen_socket);
        void Stop();

        void SetPort(std::uint16_t port) { port_ = port; }

        void RegisterHttpRequestHandler(const std::string& path, HttpMethod method,
            const HttpRequestHandler_t& callback) {
            request_handlers_[Uri(path)].insert(std::make_pair(method, callback));
        }
        void RegisterHttpRequestHandler(const Uri& uri, HttpMethod method,
            const HttpRequestHandler_t& callback) {
            request_handlers_[uri].insert(std::make_pair(method, callback));
        }
        HttpResponse HandleHttpRequest(const HttpRequest& request);

        std::string host() const { return host_; }
        std::uint16_t port() const { return port_; }
        bool running() const { return running_; }
    };
}
#endif //HTTP_SERVER_H