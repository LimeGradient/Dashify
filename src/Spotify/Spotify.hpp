#pragma once

#include <future>

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>

#include "Playback.hpp"

using namespace geode::prelude;

class Spotify {
public:
    Spotify() {}

    void init();
    Playback* getCurrentPlayback();

    bool m_isWebserverCreated = false;
protected:
    std::string spotifyToken;
    std::string accessToken;
    std::thread m_webserverThread;
    EventListener<web::WebTask> m_webListener;
};