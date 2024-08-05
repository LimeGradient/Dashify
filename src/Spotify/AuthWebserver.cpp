#include "AuthWebserver.hpp"

void Webserver::createServer() {
    this->m_spotify->m_isWebserverCreated = true;

    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([](){
        return "Hello world";
    });

    app.port(18080).multithreaded().run_async();
}