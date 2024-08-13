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
    void getAccessToken(std::string code);

    std::string getToken();

    // control playback
    void pausePlayback();
    void resumePlayback();

    void skipSong();
    void previousSong();

    Playback* getCurrentPlayback();

    bool m_isWebserverCreated;
    bool m_getAccessToken;
    bool m_isPlaybackGETFinished = false;
    std::string clientID = "32c1a06b2faa4414aa19f3b4caa06a47";
protected:
    std::string spotifyToken = "MDRkNTcxMWFjZWRkNGE2NWE1N2YzYjI5YmYwZWNmNjU=";
    std::string accessToken;
    std::thread m_webserverThread;
    EventListener<web::WebTask> m_webListener;
    EventListener<web::WebTask> m_webListener2;
    EventListener<web::WebTask> m_refreshTokenListener;
};