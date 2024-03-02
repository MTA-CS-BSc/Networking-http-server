#define _CRT_SECURE_NO_WARNINGS

#include "HttpServer.h"
#include "MtaRequestHandlers.h"
#include "Settings.h"

using mta_http_server::HttpServer;
using mta_http_server::Settings;

int main() {
    HttpServer server = HttpServer(Settings::HOST, Settings::PORT);
    RegisterMTAHandlers(server);

    try {
        server.Start();
        server.Stop();
    } catch (const std::exception& e) {
        std::cout << "Server could not be started: " << e.what() << std::endl;
    }

    return 0;
}