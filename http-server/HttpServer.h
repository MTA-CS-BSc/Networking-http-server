#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <time.h>
#include <iostream>
#include "Settings.h"
#include <functional>
#include "HttpMessage.h"
#include "Uri.h"
#include "SingletonHtmlPlaceholder.h"

using mta_http_server::HttpRequest;
using mta_http_server::HttpMethod;
using mta_http_server::Settings;

namespace mta_http_server {
    // A request handler should expect a request as argument and returns a response
    using HttpRequestHandler_t = std::function<HttpResponse(const HttpRequest&)>;

    // Defines a state of a socket as was shown in class
    typedef struct socket_state_t {
        SOCKET id;			// Socket handle
        int	recv;			// Receiving?
        int	send;			// Sending?
        char buffer[Settings::MAX_BUFFER_SIZE];
        int len;
    } SOCKET_STATE;

    typedef std::map<Uri, std::map<HttpMethod, HttpRequestHandler_t>> request_handlers_t;
    typedef class HttpServer;

    enum SocketFunction {
        EMPTY,
        LISTEN,
        RECEIVE,
        IDLE,
        SEND
    };

    // Default request handlers
    class DefaultRequestHandlers {
    private:
        request_handlers_t request_handlers_;
        
        void RegisterHttpRequestHandler(const Uri& uri, HttpMethod method,
            const HttpRequestHandler_t& callback) {
            if (method == HttpMethod::OPTIONS ||
                method == HttpMethod::TRACE ||
                method == HttpMethod::HEAD)
                throw std::logic_error("Can't assign to functions with default behaviour");

            request_handlers_[uri].insert(std::make_pair(method, callback));
        }

        void RegisterHttpRequestHandler(const std::string& path, HttpMethod method,
            const HttpRequestHandler_t& callback) {
            RegisterHttpRequestHandler(Uri(path), method, callback);
        }

        HttpRequestHandler_t dummy_handler = [](const HttpRequest& request) -> HttpResponse {
            return HttpResponse();
        };

        void RegisterDummy(const Uri& uri, HttpMethod method) {
            request_handlers_[uri].insert(std::make_pair(method, dummy_handler));
        }
        void RegisterDummy(const std::string& path, HttpMethod method) {
            RegisterDummy(Uri(path), method);
        }

        HttpRequestHandler_t get_health = [](const HttpRequest& request) -> HttpResponse {
            HttpResponse response = HttpResponse(HttpStatusCode::Ok);

            response.SetHeader("Content-Type", "text/plain");
            response.SetContent("OK");

            return response;
        };
    public:
        const request_handlers_t& request_handlers() const { return request_handlers_; }
        DefaultRequestHandlers() {
            RegisterHttpRequestHandler("/health", HttpMethod::GET, get_health);
            RegisterDummy("/health", HttpMethod::HEAD);
            RegisterDummy("/health", HttpMethod::TRACE);
            RegisterDummy("/health", HttpMethod::OPTIONS);
        }
    };

    // A handler for server sockets
    class SocketService {
    private:
        sockaddr_in server_service_;
        SOCKET_STATE sockets_[Settings::MAX_SOCKETS];
        int sockets_amount_;
        int max_sockets_;
        HttpServer* parent_;
    public:
        SocketService(HttpServer* parent) :
            parent_(parent), server_service_(),
            max_sockets_(Settings::MAX_SOCKETS),
            sockets_amount_(0), sockets_() { }
        ~SocketService() = default;
        SocketService(SocketService&&) = default;
        SOCKET_STATE* findListeningSocket();
        bool addSocket(SOCKET id, SocketFunction what);
        void removeSocket(int index);
        void acceptConnection(int index);
        void receiveMessage(int index);
        void sendMessage(int index);

        void SetSocketsAmount(int value) { sockets_amount_ = value; }
        void SetServerService(ADDRESS_FAMILY sin_family, ULONG addr, uint16_t port) {
            server_service_.sin_family = sin_family;
            server_service_.sin_addr.s_addr = addr;
            server_service_.sin_port = htons(port);
        }

        const SOCKET_STATE* sockets() const { return sockets_;  }
        const int max_sockets() const { return max_sockets_;  }
        const int sockets_amount() const { return sockets_amount_; }
        const sockaddr_in& server_service() const { return server_service_; }
    };

    // An HTTP Server class for sockets and requests management
	class HttpServer {
    private:
        std::string host_;
        std::uint16_t port_;
        bool running_;
        request_handlers_t request_handlers_;
        SocketService socket_service_;

        // Default head, trace, options
        HttpResponse handleOptionsRequest(const HttpRequest&);
        HttpResponse handleHeadRequest(const HttpRequest&);
        std::vector<HttpMethod> getMethodsForURI(const Uri&);
        HttpResponse handleTraceRequest(const HttpRequest&);
    public:
        HttpServer() = default;
        HttpServer(const HttpServer&) = default;
        HttpServer(HttpServer&&) = default;
        HttpServer& operator=(HttpServer&&) = default;
        ~HttpServer() = default;

        explicit HttpServer(const std::string& host, std::uint16_t port) : 
            host_(host), port_(port), running_(false),
            request_handlers_(DefaultRequestHandlers().request_handlers()), socket_service_(SocketService(this)) {
            SingletonHtmlPlaceholder::getInstance().set("Guest");
        }

        void Start();
        void Listen(SOCKET& listen_socket);
        inline void Stop() { running_ = false; }
        
        void RegisterHttpRequestHandler(const std::string& path, HttpMethod method,
            const HttpRequestHandler_t& callback) { request_handlers_[Uri(path)].insert(std::make_pair(method, callback)); }
        void RegisterHttpRequestHandler(const Uri& uri, HttpMethod method,
            const HttpRequestHandler_t& callback) { request_handlers_[uri].insert(std::make_pair(method, callback)); }

        void ProcessEvents();
        HttpResponse HandleHttpRequest(const HttpRequest& request);
        inline void SetPort(std::uint16_t port) { port_ = port; }

        const SOCKET_STATE* sockets() const { return socket_service_.sockets(); }
        const int max_sockets() const { return socket_service_.max_sockets(); }
        const std::string host() const { return host_; }
        const std::uint16_t port() const { return port_; }
        const bool running() const { return running_; }
    };
}
#endif //HTTP_SERVER_H_