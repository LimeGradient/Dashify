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

protected:
    std::string spotifyToken;
    std::string accessToken;
    EventListener<web::WebTask> m_webListener;
};