#ifndef REQUEST_HANDLERS_H_
#define REQUEST_HANDLERS_H

using mta_http_server::HttpMethod;
using mta_http_server::HttpRequestHandler_t;

namespace mta_http_server {
	class DefaultHandlers {
	private:
		request_handlers_t request_handlers_;

		void RegisterHttpRequestHandler(const std::string& path, HttpMethod method,
			const HttpRequestHandler_t& callback) {
			request_handlers_[Uri(path)].insert(std::make_pair(method, callback));
		}

		void RegisterHttpRequestHandler(const Uri& uri, HttpMethod method,
			const HttpRequestHandler_t& callback) {
			request_handlers_[uri].insert(std::make_pair(method, callback));
		}
	public:
		const request_handlers_t& request_handlers() const { return request_handlers_; }
		DefaultHandlers() : request_handlers_(request_handlers_t()) {} //TODO: Not implemented
	};
}
#endif //REQUEST_HANDLERS_H