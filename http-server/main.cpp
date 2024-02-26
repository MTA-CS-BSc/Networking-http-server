#include <cerrno>
#include <chrono>
#include <cstring>
#include <iostream>
#include <string>

#include "HttpServer.h"
#include "Uri.h"
#include "Settings.h"

using mta_http_server::HttpServer;
using mta_http_server::Settings;

int main() {
    HttpServer server = HttpServer(Settings::HOST, Settings::PORT);
    return 0;
}