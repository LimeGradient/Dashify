#include "AuthWebserver.hpp"

void Webserver::createServer() {
    this->m_spotify->m_isWebserverCreated = true;

    DataManager* dm = new DataManager();

    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([](){
        return "Hello SpotiDash";
    });

    CROW_ROUTE(app, "/login")([=,this](const crow::request &req, crow::response &res) {
        std::string state = Webserver::generateRandomString(16);

        res.redirect(fmt::format("https://accounts.spotify.com/authorize?response_type=code&client_id={}&scope=user-read-private%20user-read-email%20user-read-playback-state%20user-modify-playback-state&redirect_uri=http%3A%2F%2Flocalhost%3A18080%2Fcallback&state={}", 
            this->m_spotify->clientID,
            state
        ));
        res.end();
    });

    CROW_ROUTE(app, "/callback")([=, this](const crow::request &req, crow::response &res) {
        std::string code = req.url_params.get("code");
        std::string state = req.url_params.get("state");

        if (state.empty()) {
            log::info("state was not found");
        } else {
            this->m_spotify->getAccessToken(code);
        }

        res.end();
    });

    app.port(18080).multithreaded().run_async();
}

std::string Webserver::generateRandomString(const int len) {
    // https://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    return tmp_s;
}