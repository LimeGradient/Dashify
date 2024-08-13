#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>

#include "Spotify/Spotify.hpp"

using namespace geode::prelude;

class Playhead : public CCNode {
protected:
    void setPlayback();

    void onLogin(CCObject*);
    void onPauseOrPlay(CCObject*);
    void onSkip(CCObject*);
    void onPrevious(CCObject*);
    void onVolumeChange(CCObject*);

    Spotify* spotify;
    bool m_isPauseButton;

    CCMenuItemSpriteExtra* m_playControlButton;
    CCLabelBMFont* m_songTitleLabel;
    CCLabelBMFont* m_songArtistsLabel;
    CCLabelBMFont* m_albumTitleLabel;
    CCSprite* m_albumCoverSprite;

    EventListener<web::WebTask> m_webListener;
public:
    bool init(Spotify* spotify);
    static Playhead* create(Spotify* spotify);
};