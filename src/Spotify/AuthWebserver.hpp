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

    Spotify* m_spotify;
};