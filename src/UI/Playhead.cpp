#include "Playhead.hpp"

bool Playhead::init(Spotify* spotify) {
    this->spotify = spotify;

    auto director = CCDirector::get();

    auto menu = CCMenu::create();
    menu->setID("dashify-playhead-menu");
    menu->setPosition({0.f, 0.f});
    this->addChild(menu);

    auto background = CCScale9Sprite::create("GJ_square01.png", {0.f, 0.f, 80.f, 80.f});
    background->setContentWidth(200.f);
    background->setPosition({director->getScreenRight() - 110.f, director->getScreenTop() - 50.f});
    background->setID("dashify-playhead-background");
    menu->addChild(background);

    auto playbackInfo = this->spotify->getCurrentPlayback();

    auto playControlButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_pauseEditorBtn_001.png"),
        this,
        menu_selector(Playhead::onPauseOrPlay)
    );
    playControlButton->setPosition({background->getPositionX(), background->getPositionY() - 15.f});
    playControlButton->setScale(0.75f);
    playControlButton->setID("dashify-play-control-button");
    menu->addChild(playControlButton);

    m_songTitleLabel = CCLabelBMFont::create("because i liked a boy", "bigFont.fnt");
    m_songTitleLabel->setPosition({background->getPositionX(), background->getPositionY() + 25.f});
    m_songTitleLabel->setScale(0.5f);
    m_songTitleLabel->setID("dashify-song-title-label");
    menu->addChild(m_songTitleLabel);

    m_albumTitleLabel = CCLabelBMFont::create("on emails i can't send", "bigFont.fnt");
    m_albumTitleLabel->setPosition({background->getPositionX(), background->getPositionY() + 10.f});
    m_albumTitleLabel->setScale(0.25f);
    m_albumTitleLabel->setID("dashify-album-title-label");
    menu->addChild(m_albumTitleLabel);
    
    this->m_isPauseButton = true;

    this->setID("dashify-playhead");
    this->setZOrder(3);

    return true;
}

void Playhead::onPauseOrPlay(CCObject* sender) {
    auto senderNode = static_cast<CCMenuItemSpriteExtra*>(sender);

    auto playButtonSprite = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
    playButtonSprite->setScale(0.5f);

    senderNode->setSprite((this->m_isPauseButton) ? playButtonSprite : CCSprite::createWithSpriteFrameName("GJ_pauseEditorBtn_001.png"));
    senderNode->setScale(0.75f);
    if (this->m_isPauseButton) {
        this->spotify->pausePlayback();
    } else {
        this->spotify->resumePlayback();
    }

    this->m_isPauseButton = !this->m_isPauseButton;
}

Playhead* Playhead::create(Spotify* spotify) {
    auto ret = new Playhead();

    if (ret && ret->init(spotify)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}