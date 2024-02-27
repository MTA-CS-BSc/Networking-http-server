#ifndef HTTP_MESSAGE_H_
#define HTTP_MESSAGE_H_

#include <map>
#include <utility>
#include <map>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <vector>
#include <numeric>

#include "Uri.h"

namespace mta_http_server {
    enum class HttpMethod {
        GET,
        HEAD,
        POST,
        PUT,
        DELETE_,
        CONNECT,
        OPTIONS,
        TRACE,
        PATCH
    };

    enum class HttpVersion {
        HTTP_0_9 = 9,
        HTTP_1_0 = 10,
        HTTP_1_1 = 11,
        HTTP_2_0 = 20
    };

    enum class HttpStatusCode {
        Continue = 100,
        SwitchingProtocols = 101,
        EarlyHints = 103,
        Ok = 200,
        Created = 201,
        Accepted = 202,
        NonAuthoritativeInformation = 203,
        NoContent = 204,
        ResetContent = 205,
        PartialContent = 206,
        MultipleChoices = 300,
        MovedPermanently = 301,
        Found = 302,
        NotModified = 304,
        BadRequest = 400,
        Unauthorized = 401,
        Forbidden = 403,
        NotFound = 404,
        MethodNotAllowed = 405,
        RequestTimeout = 408,
        ImATeapot = 418,
        InternalServerError = 500,
        NotImplemented = 501,
        BadGateway = 502,
        ServiceUnvailable = 503,
        GatewayTimeout = 504,
        HttpVersionNotSupported = 505
    };

    // Utility functions to convert between string or integer to enum classes
    std::string to_string(HttpMethod method);
    std::string to_string(HttpVersion version);
    std::string to_string(HttpStatusCode status_code);
    HttpMethod string_to_method(const std::string& method_string);
    HttpVersion string_to_version(const std::string& version_string);

    // Defines the common interface of an HTTP request and HTTP response.
    // Each message will have an HTTP version, collection of header fields,
    // and message content. The collection of headers and content can be empty.
    class HttpMessageInterface {
    protected:
        HttpVersion version_;
        std::map<std::string, std::string> headers_;
        std::string content_;

        void SetContentLength() {
            SetHeader("Content-Length", std::to_string(content_.length()));
        }

    public:
        HttpMessageInterface() : version_(HttpVersion::HTTP_1_1) { SetContentLength(); }
        virtual ~HttpMessageInterface() = default;

        void SetHeader(const std::string& key, const std::string& value) {
            headers_[key] = value;
        }
        void RemoveHeader(const std::string& key) { headers_.erase(key); }
        void ClearHeader() { headers_.clear(); }
        void SetContent(const std::string& content) {
            content_ = content;
            SetContentLength();
        }
        void ClearContent() {
            content_.clear();
            SetContentLength();
        }

        HttpVersion version() const { return version_; }
        std::string header(const std::string& key) const {
            if (headers_.count(key) > 0) return headers_.at(key);
            return {};
        }
        std::map<std::string, std::string> headers() const { return headers_; }
        std::string content() const { return content_; }
        size_t content_length() const { return content_.length(); }
    };

    // A QueryParams object represents the query params for a single HTTP request.
    // By default, HTTP requests without query parameters will have an empty map.
    class QueryParams {
    protected:
        std::map<std::string, std::string> params_;
    public:
        QueryParams() : params_() { }
        QueryParams(const std::map<std::string, std::string> params) : params_(params) { }
        ~QueryParams() = default;
        void SetParam(const std::string& key, const std::string& value) {
            params_[key] = value;
        }
        void RemoveParam(const std::string& key) { params_.erase(key); }
        void ClearParams() { params_.clear(); }
        std::map<std::string, std::string> params() const { return params_; }
    };

    // An HttpRequest object represents a single HTTP request
    // It has a HTTP method and URI so that the server can identify
    // the corresponding resource and action
    class HttpRequest : public HttpMessageInterface {
    private:
        HttpMethod method_;
        Uri uri_;
        QueryParams params_;
    public:
        HttpRequest() : method_(HttpMethod::GET) {}
        ~HttpRequest() override = default;

        void SetMethod(HttpMethod method) { method_ = method; }
        void SetUri(const Uri& uri) { uri_ = uri; }
        void SetQueryParams(const QueryParams& params) { params_ = params; }

        QueryParams params() const { return params_.params(); }
        HttpMethod method() const { return method_; }
        Uri uri() const { return uri_; }

        friend std::string to_string(const HttpRequest& request);
        friend HttpRequest string_to_request(const std::string& request_string);
    };

    // An HTTPResponse object represents a single HTTP response
    // The HTTP server sends an HTTP response to a client that include
    // an HTTP status code, headers, and (optional) content
    class HttpResponse : public HttpMessageInterface {
    private:
        HttpStatusCode status_code_;
    public:
        HttpResponse() : status_code_(HttpStatusCode::Ok) { SetHeader("Server", "Apache/2.4.41 (Unix)"); }
        explicit HttpResponse(HttpStatusCode status_code) : status_code_(status_code) { SetHeader("Server", "Apache/2.4.41 (Unix)"); }
        ~HttpResponse() override = default;

        void SetStatusCode(HttpStatusCode status_code) { status_code_ = status_code; }

        HttpStatusCode status_code() const { return status_code_; }

        friend std::string to_string(const HttpResponse& request, bool send_content);
        friend HttpResponse string_to_response(const std::string& response_string);
    };

    // Utility functions to convert HTTP message objects to string and vice versa
    std::string to_string(const std::vector<HttpMethod>& methods);
    std::string to_string(const HttpRequest& request);
    std::string to_string(const HttpResponse& response, bool send_content = true);
    HttpRequest string_to_request(const std::string& request_string);
    HttpResponse string_to_response(const std::string& response_string);
    std::pair<std::string, std::map<std::string, std::string>> parseURI(const std::string& uri);
    std::string get_timestamp_for_response();
}
#endif  // HTTP_MESSAGE_H_