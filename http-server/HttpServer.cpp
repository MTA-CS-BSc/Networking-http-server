#include "HttpServer.h"

namespace mta_http_server {
    HttpResponse HttpServer::HandleHttpRequest(const HttpRequest& request) {
        auto it = request_handlers_.find(request.uri());
        if (it == request_handlers_.end()) {  // this uri is not registered
            return HttpResponse(HttpStatusCode::NotFound);
        }
        auto callback_it = it->second.find(request.method());

        if (callback_it == it->second.end()) {  // no handler for this method
            return HttpResponse(HttpStatusCode::MethodNotAllowed);
        }
        return callback_it->second(request);  // call handler to process the request
    }
}