#include "HttpServer.h"

namespace mta_http_server {
	bool SocketService::addSocket(SOCKET id, SocketFunction what) {
		for (int i = 0; i < max_sockets(); i++) {
			if (sockets_[i].recv == SocketFunction::EMPTY) {
				sockets_[i].id = id;
				sockets_[i].recv = what;
				sockets_[i].send = SocketFunction::IDLE;
				sockets_[i].len = 0;
				sockets_amount_++;
				return true;
			}
		}
		return false;
	}

	void SocketService::removeSocket(int index) {
		sockets_[index].recv = EMPTY;
		sockets_[index].send = EMPTY;
		sockets_amount_--;
	}

	void SocketService::acceptConnection(int index) {
		SOCKET id = sockets_[index].id;
		struct sockaddr_in from;		// Address of sending partner
		int fromLen = sizeof(from);

		SOCKET msgSocket = accept(id, (struct sockaddr*)&from, &fromLen);

		if (INVALID_SOCKET == msgSocket)
			throw std::runtime_error("Server: Error at accept(): " + WSAGetLastError());

		std::cout << "Server: Client " << inet_ntoa(from.sin_addr) << ":" << ntohs(from.sin_port) << " is connected." << std::endl;

		unsigned long flag = 1;
		if (ioctlsocket(msgSocket, FIONBIO, &flag) != 0)
			throw std::runtime_error("Server: Error at accept(): " + WSAGetLastError());

		if (!addSocket(msgSocket, RECEIVE)) {
			std::cout << "\t\tToo many connections, dropped!\n";
			closesocket(id);
		}

		return;
	}

	void SocketService::receiveMessage(int index) {
		SOCKET msgSocket = sockets_[index].id;

		int len = sockets_[index].len;
		int bytesRecv = recv(msgSocket, &sockets_[index].buffer[len], sizeof(sockets_[index].buffer) - len, 0);

		if (SOCKET_ERROR == bytesRecv) {
			std::cout << "Server: Error at recv(): " << WSAGetLastError() << std::endl;
			closesocket(msgSocket);
			removeSocket(index);
			return;
		}

		if (bytesRecv == 0) {
			closesocket(msgSocket);
			removeSocket(index);
			return;
		}

		else {
			// Get message from socket
			sockets_[index].buffer[len + bytesRecv] = '\0'; //add the null-terminating to make it a string
			std::cout << "Server: Recieved: " << bytesRecv << " bytes of \"" << &sockets_[index].buffer[len] << "\" message.\n\n";
			sockets_[index].len += bytesRecv;

			// Convert to HttpRequest
			//TODO: Add try/catch
			HttpRequest request = string_to_request(sockets_[index].buffer);

			// Handle request
			HttpResponse response = parent_->HandleHttpRequest(request);

			// Convert response to string for socket
			std::string string_response = to_string(response);

			// Copy response data to corresponding buffer
			sockets_[index].recv = EMPTY;
			sockets_[index].send = SEND;

			std::fill(std::begin(sockets_[index].buffer), std::end(sockets_[index].buffer), '\0');
			memcpy(sockets_[index].buffer, string_response.c_str(), string_response.length());
		}
	}

	void SocketService::sendMessage(int index) {
		int bytesSent = 0;

		SOCKET msgSocket = sockets_[index].id;
		bytesSent = send(msgSocket, sockets_[index].buffer, strlen(sockets_[index].buffer), 0);

		if (SOCKET_ERROR == bytesSent) {
			std::cout << "Time Server: Error at send(): " << WSAGetLastError() << std::endl;
			return;
		}

		std::cout << "Server: Sent: " << bytesSent << "\\" << strlen(sockets_[index].buffer) << " bytes of \"" << sockets_[index].buffer << "\" message.\n\n";
		sockets_[index].send = IDLE;
	}

	void HttpServer::Start() {
		WSAData wsa_data;

		if (!port())
			throw std::runtime_error("Port is not configured!");

		if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsa_data))
			throw std::runtime_error("Error at WSAStartup");

		SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (INVALID_SOCKET == listen_socket) {
			int last_err = WSAGetLastError();
			WSACleanup();
			throw std::runtime_error("Error at socket(): " + last_err);
		}

		socket_service_.SetServerService(AF_INET, INADDR_ANY, port());

		if (SOCKET_ERROR == bind(listen_socket, (SOCKADDR*)&socket_service_.server_service(), sizeof(socket_service_.server_service()))) {
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

		socket_service_.addSocket(listen_socket, LISTEN);
		running_ = true;
	}

	std::vector<HttpMethod> HttpServer::getMethodsForURI(const Uri& uri) {
		std::vector<HttpMethod> methods;
		auto it = request_handlers_.find(uri);

		if (it != request_handlers_.end()) {
			auto methods_map_for_uri = it->second;

			for (const auto& pair : methods_map_for_uri)
				methods.push_back(pair.first);
		}

		return methods;
	}

	HttpResponse HttpServer::handleOptionsRequest(const HttpRequest& request) {
		HttpResponse response = HttpResponse(HttpStatusCode::NoContent);
		std::vector<HttpMethod> allow_methods = getMethodsForURI(request.uri());
		std::string methods_string = to_string(allow_methods);
		response.SetHeader("Allow", methods_string);

		return response;
	}
	
	HttpResponse HttpServer::handleHeadRequest(const HttpRequest& request) {
		HttpRequest c_request(request);
		c_request.SetMethod(HttpMethod::GET);
		
		HttpResponse respone = HandleHttpRequest(c_request);

		if (respone.status_code() == HttpStatusCode::Ok)
			respone.ClearContent();

		return respone;
	}

	HttpResponse HttpServer::handleTraceRequest(const HttpRequest& request) {
		HttpResponse response = HttpResponse(HttpStatusCode::Ok);
		response.SetHeader("Content-Type", "message/http");
		response.SetContent(to_string(request));
		return response;
	}

	HttpResponse HttpServer::HandleHttpRequest(const HttpRequest& request) {
		auto it = request_handlers_.find(request.uri());
		if (it == request_handlers_.end())  // this uri is not registered
			return HttpResponse(HttpStatusCode::NotFound);

		auto callback_it = it->second.find(request.method());
		
		if (callback_it == it->second.end()) // no handler for this method
			return HttpResponse(HttpStatusCode::MethodNotAllowed);

		switch (request.method()) {
		case HttpMethod::OPTIONS:
			return handleOptionsRequest(request);
		case HttpMethod::TRACE:
			return handleTraceRequest(request);
		case HttpMethod::HEAD:
			return handleHeadRequest(request);
		default:
			return callback_it->second(request);
		}
	}

	SOCKET_STATE* SocketService::findListeningSocket() {
		for (auto& item : sockets_) {
			if (item.recv == SocketFunction::LISTEN)
				return &item;
		}

		return nullptr;
	}

	void HttpServer::ProcessEvents() {
		while (running_) {
			fd_set waitRecv;
			FD_ZERO(&waitRecv);
			for (int i = 0; i < max_sockets(); i++) {
				if ((sockets()[i].recv == LISTEN) || (sockets()[i].recv == RECEIVE))
					FD_SET(sockets()[i].id, &waitRecv);
			}

			fd_set waitSend;
			FD_ZERO(&waitSend);

			for (int i = 0; i < max_sockets(); i++) {
				if (sockets()[i].send == SEND)
					FD_SET(sockets()[i].id, &waitSend);
			}

			int nfd;
			nfd = select(0, &waitRecv, &waitSend, NULL, NULL);
			if (nfd == SOCKET_ERROR) {
				std::cout << "Server: Error at select(): " << WSAGetLastError() << std::endl;
				WSACleanup();
				return;
			}

			for (int i = 0; i < max_sockets() && nfd > 0; i++) {
				if (FD_ISSET(sockets()[i].id, &waitRecv)) {
					nfd--;
					switch (sockets()[i].recv) {
					case LISTEN:
						socket_service_.acceptConnection(i);
						break;

					case RECEIVE:
						socket_service_.receiveMessage(i);
						break;
					}
				}
			}

			for (int i = 0; i < max_sockets() && nfd > 0; i++) {
				if (FD_ISSET(sockets()[i].id, &waitSend)) {
					nfd--;
					switch (sockets()[i].send) {
					case SEND:
						socket_service_.sendMessage(i);
						break;
					}
				}
			}
		}

		closesocket(socket_service_.findListeningSocket()->id);
		WSACleanup();
	}
}