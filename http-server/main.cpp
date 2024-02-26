#include <cerrno>
#include <chrono>
#include <cstring>
#include <iostream>
#include <string>

#include "HttpMessage.h"
#include "Uri.h"

//using simple_http_server::HttpMethod;
//using simple_http_server::HttpRequest;
//using simple_http_server::HttpResponse;
//using simple_http_server::HttpServer;
//using simple_http_server::HttpStatusCode;

using HttpServer::HttpRequest;
using HttpServer::HttpMessageInterface;

int main(void) {
    std::string host = "0.0.0.0";
    int port = 8080;
    
    std::string request_test = "GET /search?q=example&limit=10 HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:97.0) Gecko/20100101 Firefox/97.0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate, br\r\nConnection: keep-alive\r\n\r\n";

    HttpRequest req = HttpServer::string_to_request(request_test);

    return 0;
}