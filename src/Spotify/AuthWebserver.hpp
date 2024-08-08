#pragma once
#undef _WINSOCKAPI_
#include <crow.h>

#include "Spotify.hpp"

class Webserver {
public:
    Webserver(Spotify* spotify) {
        this->m_spotify = spotify;
    }

    void createServer();
    static std::string generateRandomString(const int len);

    Spotify* m_spotify;
};