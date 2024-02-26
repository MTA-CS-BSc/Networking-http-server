#include "HttpServer.h"

namespace mta_http_server {
    void HttpServer::Start() {
		WSAData wsa_data;

		if (!port)
			throw std::runtime_error("Port is not configured!");

		if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsa_data))
			throw std::runtime_error("Error at WSAStartup");

		SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		
		if (INVALID_SOCKET == listen_socket) {
			int last_err = WSAGetLastError();
			WSACleanup();
			throw std::runtime_error("Error at socket(): " + last_err);
		}

		server_service_.sin_family = AF_INET;
		server_service_.sin_addr.s_addr = INADDR_ANY;
		server_service_.sin_port = htons(port_);

		if (SOCKET_ERROR == bind(listen_socket, (SOCKADDR*)&server_service_, sizeof(server_service_)))
		{
			int last_err = WSAGetLastError();
			closesocket(listen_socket);
			WSACleanup();
			throw std::runtime_error("Error at bind(): " + last_err);
		}

		Listen(listen_socket);
    }

	void HttpServer::Listen(SOCKET& listen_socket) {
		if (SOCKET_ERROR == listen(listen_socket, 5)) {
			int last_err = WSAGetLastError();
			closesocket(listen_socket);
			WSACleanup();
			throw std::runtime_error("Time Server: Error at listen(): " + last_err);
		}

		// TODO: Add missing function
		/*addSocket(listen_socket_, LISTEN);
		running_ = true; */
	}

    HttpResponse HttpServer::HandleHttpRequest(const HttpRequest& request) {
        auto it = request_handlers_.find(request.uri());
        if (it == request_handlers_.end()) {  // this uri is not registered
            return HttpResponse(HttpStatusCode::NotFound);
        }
        auto callback_it = it->second.find(request.method());

        if (callback_it == it->second.end()) {  // no handler for this method
            return HttpResponse(HttpStatusCode::MethodNotAllowed);
        }
        return callback_it->second(request);  // call handler to process the request
    }

	SOCKET_STATE* HttpServer::findListeningSocket() {
		for (auto& item : sockets_) {
			if (item.recv == SOCK_FUNC::LISTEN)
				return &item;
		}

		return nullptr;
	}

	void HttpServer::ProcessEvents() {
		while (running_) {
			//TODO: Not implemented
		}

		closesocket(findListeningSocket()->id);
		WSACleanup();
	}
}