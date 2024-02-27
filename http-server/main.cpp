#define _CRT_SECURE_NO_WARNINGS

#include <cerrno>
#include <chrono>
#include <cstring>
#include <iostream>
#include <string>

#include "HttpServer.h"
#include "Uri.h"
#include "Settings.h"

using mta_http_server::HttpServer;
using mta_http_server::DefaultHandlers;
using mta_http_server::Settings;

int main() {
    HttpServer server = HttpServer(Settings::HOST, Settings::PORT);
    server.SetRequestHandlers(DefaultHandlers());

    server.Start();
    server.ProcessEvents();
    server.Stop();

    return 0;
}