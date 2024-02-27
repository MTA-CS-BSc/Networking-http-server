#define _CRT_SECURE_NO_WARNINGS

#include "HttpServer.h"
#include "Settings.h"

using mta_http_server::HttpServer;
using mta_http_server::DefaultRequestHandlers;
using mta_http_server::Settings;

int main() {
    HttpServer server = HttpServer(Settings::HOST, Settings::PORT);
    server.SetRequestHandlers(DefaultRequestHandlers());

    server.Start();
    server.ProcessEvents();
    server.Stop();

    return 0;
}