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

		socket_service_.addSocket(listen_socket, LISTEN);
		running_ = true;
	}

	HttpResponse HttpServer::HandleHttpRequest(const HttpRequest& request) {
		auto it = request_handlers_.find(request.uri());
		if (it == request_handlers_.end())  // this uri is not registered
			return HttpResponse(HttpStatusCode::NotFound);

		auto callback_it = it->second.find(request.method());

		if (callback_it == it->second.end()) // no handler for this method
			return HttpResponse(HttpStatusCode::MethodNotAllowed);

		return callback_it->second(request);  // call handler to process the request
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
			//TODO: Not implemented
		}

		closesocket(socket_service_.findListeningSocket()->id);
		WSACleanup();
	}

	bool SocketService::addSocket(SOCKET id, SocketFunction what) {
		for (int i = 0; i < Settings::MAX_SOCKETS; i++) {
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

	void SocketService::removeSocket(int index)
	{
		sockets_[index].recv = EMPTY;
		sockets_[index].send = EMPTY;
		sockets_amount_--;
	}

	void SocketService::acceptConnection(int index)
	{
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
			sockets_[index].buffer[len + bytesRecv] = '\0'; //add the null-terminating to make it a string
			std::cout << "Server: Recieved: " << bytesRecv << " bytes of \"" << &sockets_[index].buffer[len] << "\" message.\n";

			sockets_[index].len += bytesRecv;

			//	if (sockets_[index].len > 0) {
			//		if (strncmp(sockets_[index].buffer, "TimeString", 10) == 0) {
			//			sockets_[index].send = SEND;
			//			//sockets_[index].send_sub_type = SEND_TIME;
			//			memcpy(sockets_[index].buffer, &sockets_[index].buffer[10], sockets_[index].len - 10);
			//			sockets_[index].len -= 10;
			//			return;
			//		}

			//		else if (strncmp(sockets_[index].buffer, "SecondsSince1970", 16) == 0) {
			//			sockets_[index].send = SEND;
			//			//sockets[index].sendSubType = SEND_SECONDS;
			//			memcpy(sockets_[index].buffer, &sockets_[index].buffer[16], sockets_[index].len - 16);
			//			sockets_[index].len -= 16;
			//			return;
			//		}

			//		else if (strncmp(sockets_[index].buffer, "Exit", 4) == 0) {
			//			closesocket(msgSocket);
			//			removeSocket(index);
			//			return;
			//		}
			//	}
			//}

		}


	}

	void SocketService::sendMessage(int index) {
		int bytesSent = 0;
		char sendBuff[255];

		SOCKET msgSocket = sockets_[index].id;

		//TODO: Implement what to send
		//if (sockets[index].sendSubType == SEND_TIME)
		//{
		//	// Answer client's request by the current time string.

		//	// Get the current time.
		//	time_t timer;
		//	time(&timer);
		//	// Parse the current time to printable string.
		//	strcpy(sendBuff, ctime(&timer));
		//	sendBuff[strlen(sendBuff) - 1] = 0; //to remove the new-line from the created string
		//}
		//else if (sockets[index].sendSubType == SEND_SECONDS)
		//{
		//	// Answer client's request by the current time in seconds.

		//	// Get the current time.
		//	time_t timer;
		//	time(&timer);
		//	// Convert the number to string.
		//	itoa((int)timer, sendBuff, 10);
		//}

		bytesSent = send(msgSocket, sendBuff, (int)strlen(sendBuff), 0);
		
		if (SOCKET_ERROR == bytesSent) {
			std::cout << "Time Server: Error at send(): " << WSAGetLastError() << std::endl;
			return;
		}

		std::cout << "Time Server: Sent: " << bytesSent << "\\" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";

		sockets_[index].send = IDLE;
	}
}