#include "AuthWebserver.hpp"

void Webserver::createServer() {
    crow::SimpleApp app;
    CROW_ROUTE(app, "/")([]() {
        return "Hello from SpotiDash!";
    });

    app.port(18080).multithreaded().run();
}