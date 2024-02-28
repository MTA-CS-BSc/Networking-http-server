#define _CRT_SECURE_NO_WARNINGS

#include "HttpServer.h"
#include "MtaRequestHandlers.h"
#include "Settings.h"

using mta_http_server::HttpServer;
using mta_http_server::Settings;

int main() {
    HttpServer server = HttpServer(Settings::HOST, Settings::PORT);
    RegisterMTAHandlers(server);

    server.Start();
    server.ProcessEvents();
    server.Stop();

    return 0;
}