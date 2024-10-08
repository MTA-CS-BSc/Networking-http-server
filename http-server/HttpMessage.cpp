#include "HttpMessage.h"

namespace mta_http_server {
    std::string string_replace(const std::string& str, const std::string& replace_from, const std::string& replace_to) {
        std::string c_str(str);

        size_t pos = c_str.find(replace_from);
        if (pos != std::string::npos)
            c_str.replace(pos, replace_from.length(), replace_to);

        return c_str;
    }

    std::string read_html_file(const std::string& path) {
        std::ifstream file(path);

        if (!file.is_open())
            throw std::runtime_error("Could not open file!");

        std::string htmlContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return htmlContent;
    }

    std::string to_string(const std::vector<HttpMethod>& methods) {
        return std::accumulate(methods.begin(), methods.end(),
            std::string(),
            [](const std::string& a, const HttpMethod& method) -> std::string {
                return a + (a.length() > 0 ? ", " : "") + to_string(method);
            });
    }

    std::string to_string(const Uri& uri) {
        return uri.path();
    }

    std::string to_string(HttpMethod method) {
        switch (method) {
        case HttpMethod::GET:
            return "GET";
        case HttpMethod::HEAD:
            return "HEAD";
        case HttpMethod::POST:
            return "POST";
        case HttpMethod::PUT:
            return "PUT";
        case HttpMethod::DEL:
            return "DELETE";
        case HttpMethod::CONNECT:
            return "CONNECT";
        case HttpMethod::OPTIONS:
            return "OPTIONS";
        case HttpMethod::TRACE:
            return "TRACE";
        case HttpMethod::PATCH:
            return "PATCH";
        default:
            return "";
        }
    }

    std::string to_string(HttpVersion version) {
        switch (version) {
        case HttpVersion::HTTP_0_9:
            return "HTTP/0.9";
        case HttpVersion::HTTP_1_0:
            return "HTTP/1.0";
        case HttpVersion::HTTP_1_1:
            return "HTTP/1.1";
        case HttpVersion::HTTP_2_0:
            return "HTTP/2.0";
        default:
            return "";
        }
    }

    std::string to_string(HttpStatusCode status_code) {
        switch (status_code) {
        case HttpStatusCode::Continue:
            return "Continue";
        case HttpStatusCode::Ok:
            return "OK";
        case HttpStatusCode::Accepted:
            return "Accepted";
        case HttpStatusCode::MovedPermanently:
            return "Moved Permanently";
        case HttpStatusCode::Found:
            return "Found";
        case HttpStatusCode::BadRequest:
            return "Bad Request";
        case HttpStatusCode::Forbidden:
            return "Forbidden";
        case HttpStatusCode::NotFound:
            return "Not Found";
        case HttpStatusCode::MethodNotAllowed:
            return "Method Not Allowed";
        case HttpStatusCode::ImATeapot:
            return "I'm a Teapot";
        case HttpStatusCode::InternalServerError:
            return "Internal Server Error";
        case HttpStatusCode::NotImplemented:
            return "Not Implemented";
        case HttpStatusCode::BadGateway:
            return "Bad Gateway";
        default:
            return "";
        }
    }

    std::string str_transform(const std::string& str, const char& method) {
        std::string transformed;
        std::transform(str.begin(), str.end(),
            std::back_inserter(transformed),
            [method](char c) { return method == 'u' ? toupper(c) : tolower(c); });

        return transformed;
    }

    HttpMethod string_to_method(const std::string& method_string) {
        std::string method_string_uppercase = str_transform(method_string, 'u');

        if (method_string_uppercase == "GET")
            return HttpMethod::GET;
        else if (method_string_uppercase == "HEAD")
            return HttpMethod::HEAD;
        else if (method_string_uppercase == "POST")
            return HttpMethod::POST;
        else if (method_string_uppercase == "PUT")
            return HttpMethod::PUT;
        else if (method_string_uppercase == "DELETE")
            return HttpMethod::DEL;
        else if (method_string_uppercase == "CONNECT")
            return HttpMethod::CONNECT;
        else if (method_string_uppercase == "OPTIONS")
            return HttpMethod::OPTIONS;
        else if (method_string_uppercase == "TRACE")
            return HttpMethod::TRACE;
        else if (method_string_uppercase == "PATCH")
            return HttpMethod::PATCH;
        else
            throw std::invalid_argument("Unexpected HTTP method");
    }

    HttpVersion string_to_version(const std::string& version_string) {
        std::string version_string_uppercase = str_transform(version_string, 'u');

        if (version_string_uppercase == "HTTP/0.9")
            return HttpVersion::HTTP_0_9;
        else if (version_string_uppercase == "HTTP/1.0")
            return HttpVersion::HTTP_1_0;
        else if (version_string_uppercase == "HTTP/1.1")
            return HttpVersion::HTTP_1_1;
        else if (version_string_uppercase == "HTTP/2" || version_string_uppercase == "HTTP/2.0")
            return HttpVersion::HTTP_2_0;
        else
            throw std::invalid_argument("Unexpected HTTP version");
    }

    QLanguage string_to_language(const std::string& lang_string) {
        std::string lang_lower_case = str_transform(lang_string, 'l');

        if (lang_string == "he")
            return QLanguage::HE;
        else if (lang_string == "en")
            return QLanguage::EN;
        else if (lang_string == "fr")
            return QLanguage::FR;
        else
            throw std::invalid_argument("Unexpected lang param");
    }

    std::string to_string(const HttpRequest& request) {
        std::ostringstream oss;

        oss << to_string(request.method()) << ' ';
        oss << request.uri().path() << ' ';
        oss << to_string(request.version()) << "\r\n";
        for (const auto& p : request.headers())
            oss << p.first << ": " << p.second << "\r\n";
        oss << "\r\n";
        oss << request.content();

        return oss.str();
    }

    std::string get_timestamp_for_response() {
        // Get the current time
        auto current_time = std::time(nullptr);
        // Convert to tm struct in GMT
        std::tm gm_time;
        std::ostringstream timestamp;

        gmtime_s(&gm_time, &current_time);
        timestamp << std::put_time(&gm_time, "%a, %d %b %Y %H:%M:%S GMT");

        return timestamp.str();
    }

    std::string to_string(const HttpResponse& response, bool send_content) {
        std::ostringstream oss;

        oss << to_string(response.version()) << ' ';
        oss << static_cast<int>(response.status_code()) << ' ';
        oss << to_string(response.status_code()) << "\r\n";
        for (const auto& p : response.headers())
            oss << p.first << ": " << p.second << "\r\n";

        oss << "Date: " << get_timestamp_for_response() << "\r\n";

        oss << "\r\n";
        if (send_content) oss << response.content();

        return oss.str();
    }

    HttpRequest string_to_request(const std::string& request_string) {
        std::string start_line, header_lines, message_body;
        std::istringstream iss;
        HttpRequest request;
        std::string line, method, path, version;  // used for first line
        std::string key, value;                   // used for header fields
        Uri uri;
        size_t lpos = 0, rpos = 0;

        rpos = request_string.find("\r\n", lpos);
        if (rpos == std::string::npos) {
            throw std::invalid_argument("Could not find request start line");
        }

        start_line = request_string.substr(lpos, rpos - lpos);
        lpos = rpos + 2;
        rpos = request_string.find("\r\n\r\n", lpos);
        if (rpos != std::string::npos) {  // has header
            header_lines = request_string.substr(lpos, rpos - lpos);
            lpos = rpos + 4;
            rpos = request_string.length();
            if (lpos < rpos) {
                message_body = request_string.substr(lpos, rpos - lpos);
            }
        }

        iss.clear();  // parse the start line
        iss.str(start_line);
        iss >> method >> path >> version;
        if (!iss.good() && !iss.eof()) {
            throw std::invalid_argument("Invalid start line format");
        }
        request.SetMethod(string_to_method(method));

        std::pair<std::string, query_params_t> parsed_path = parseURI(path);
        
        request.SetUri(Uri(parsed_path.first));
        request.SetQueryParams(parsed_path.second);

        if (string_to_version(version) != request.version()) {
            throw std::logic_error("HTTP version not supported");
        }

        iss.clear();  // parse header fields
        iss.str(header_lines);
        while (std::getline(iss, line)) {
            std::istringstream header_stream(line);
            std::getline(header_stream, key, ':');
            std::getline(header_stream, value);

            // remove whitespaces from the two strings
            key.erase(std::remove_if(key.begin(), key.end(),
                [](char c) { return std::isspace(c); }),
                key.end());
            value.erase(std::remove_if(value.begin(), value.end(),
                [](char c) { return std::isspace(c); }),
                value.end());
            request.SetHeader(key, value);
        }

        request.SetContent(message_body);

        return request;
    }

    HttpResponse string_to_response(const std::string& response_string) {
        throw std::logic_error("Method not implemented");
    }

    std::pair<std::string, query_params_t> parseURI(const std::string& uri) {
        std::string baseURI;
        query_params_t params;

        // Finding the position of '?' to separate base URI and query parameters
        std::size_t queryPos = uri.find('?');
        if (queryPos != std::string::npos) {
            baseURI = uri.substr(0, queryPos); // Extracting base URI

            // Extracting and parsing query parameters
            std::string queryParams = uri.substr(queryPos + 1);
            std::size_t start = 0, end;
            while ((end = queryParams.find('&', start)) != std::string::npos) {
                std::string param = queryParams.substr(start, end - start);
                std::size_t equalPos = param.find('=');
                if (equalPos != std::string::npos) {
                    std::string key = param.substr(0, equalPos);
                    std::string value = param.substr(equalPos + 1);
                    params[key] = value;
                }
                start = end + 1;
            }

            // Processing the last parameter (or the only parameter if there's only one)
            std::string lastParam = queryParams.substr(start);
            std::size_t equalPos = lastParam.find('=');
            if (equalPos != std::string::npos) {
                std::string key = lastParam.substr(0, equalPos);
                std::string value = lastParam.substr(equalPos + 1);
                params[key] = value;
            }
        } else
            baseURI = uri; // No query parameters, URI is just the base

        return {baseURI, params};
    }
}