#ifndef HTTP_SERVER_SETTINGS_H_
#define HTTP_SERVER_SETTINGS_H_

#include <string>
#include <winsock2.h>

namespace mta_http_server {
	class Settings {
	public:
		static constexpr size_t MAX_BUFFER_SIZE = 4096;
		static constexpr unsigned short PORT = 8080;
		static constexpr int BACK_LOG_SIZE = 1000;
		static constexpr char HOST[] = "0.0.0.0";
		static constexpr int K_MAX_CONNECTIONS = 10000;
		static constexpr int K_THREAD_POOL_SIZE = 5;
	};
}
#endif