#ifndef HTTP_SERVER_SETTINGS_H_
#define HTTP_SERVER_SETTINGS_H_

namespace mta_http_server {
	class Settings {
	public:
		static constexpr size_t MAX_BUFFER_SIZE = 4096;
		static constexpr unsigned short PORT = 8080;
		static constexpr char HOST[] = "0.0.0.0";
		static constexpr int MAX_SOCKETS = 100;
	};
}
#endif