#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>

#include "Spotify/Spotify.hpp"

using namespace geode::prelude;

class Playhead : public CCNode {
protected:
    bool init(Spotify* spotify);

    void onPauseOrPlay(CCObject*);

    Spotify* spotify;
    bool m_isPauseButton;

    CCLabelBMFont* m_songTitleLabel;
    CCLabelBMFont* m_songArtistsLabel;
    CCLabelBMFont* m_albumTitleLabel;
    CCSprite* m_albumCoverSprite;

    EventListener<web::WebTask> m_webListener;
public:
    static Playhead* create(Spotify* spotify);
};