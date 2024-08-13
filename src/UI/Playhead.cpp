#include "Playhead.hpp"

EventListener<web::WebTask> m_playbackListener;
EventListener<web::WebTask> m_imageDataListener;

bool Playhead::init(Spotify* spotify) {
    this->spotify = spotify;

    auto director = CCDirector::get();

    auto menu = CCMenu::create();
    menu->setID("dashify-playhead-menu");
    menu->setPosition({0.f, 0.f});
    this->addChild(menu);

    auto background = CCScale9Sprite::create("GJ_square01.png", {0.f, 0.f, 80.f, 80.f});
    background->setContentSize({220.f, background->getContentHeight() + 25.f});
    background->setPosition({director->getScreenRight() - 115.f, director->getScreenTop() - 60.f});
    background->setID("dashify-playhead-background");
    menu->addChild(background);

    if (Mod::get()->getSavedValue<std::string>("refresh_token").empty()) {
        auto loginButtonSprite = ButtonSprite::create("Login", "bigFont.fnt", "GJ_button_01.png");
        auto loginButton = CCMenuItemSpriteExtra::create(
            loginButtonSprite,
            this,
            menu_selector(Playhead::onLogin)
        );
        loginButton->setPosition(background->getPosition());
        loginButton->setScale(0.65f);
        loginButton->setID("dashify-login-button");
        menu->addChild(loginButton);
    } else {
        m_playControlButton = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_pauseEditorBtn_001.png"),
            this,
            menu_selector(Playhead::onPauseOrPlay)
        );
        m_playControlButton->setPosition({background->getPositionX(), background->getPositionY() - 30.f});
        m_playControlButton->setScale(0.75f);
        m_playControlButton->setID("dashify-play-control-button");
        menu->addChild(m_playControlButton);

        auto skipSongButton = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("fast-forward-button.png"_spr),
            this,
            menu_selector(Playhead::onSkip)
        );
        skipSongButton->setPosition({m_playControlButton->getPositionX() + 40.f, m_playControlButton->getPositionY()});
        skipSongButton->setScale(0.6f);
        skipSongButton->setID("dashify-skip-song-button");
        menu->addChild(skipSongButton);

        auto previousSongButton = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("back-button.png"_spr),
            this,
            menu_selector(Playhead::onPrevious)
        );
        previousSongButton->setPosition({m_playControlButton->getPositionX() - 40.f, m_playControlButton->getPositionY()});
        previousSongButton->setScale(0.6f);
        previousSongButton->setID("dashify-previous-song-button");
        menu->addChild(previousSongButton);

        m_songTitleLabel = CCLabelBMFont::create("", "bigFont.fnt");
        m_songTitleLabel->setPosition({background->getPositionX() + 30.f, background->getPositionY() + 35.f});
        m_songTitleLabel->setScale(0.25f);
        m_songTitleLabel->setID("dashify-song-title-label");
        menu->addChild(m_songTitleLabel);

        m_albumTitleLabel = CCLabelBMFont::create("", "bigFont.fnt");
        m_albumTitleLabel->setPosition({background->getPositionX() + 30.f, background->getPositionY() + 22.5f});
        m_albumTitleLabel->setScale(0.25f);
        m_albumTitleLabel->setID("dashify-album-title-label");
        menu->addChild(m_albumTitleLabel);

        m_songArtistsLabel = CCLabelBMFont::create("", "bigFont.fnt");
        m_songArtistsLabel->setPosition({background->getPositionX() + 30.f, background->getPositionY() + 10.f});
        m_songArtistsLabel->setScale(0.25f);
        m_songArtistsLabel->setID("dashify-song-artists-label");
        menu->addChild(m_songArtistsLabel);

        m_albumCoverSprite = CCSprite::createWithSpriteFrameName("default-album-cover.png"_spr);
        m_albumCoverSprite->setPosition({background->getPositionX() - 70.f, background->getPositionY() + 15.f});
        m_albumCoverSprite->setScale(0.75f);
        m_albumCoverSprite->setID("dashify-album-cover-sprite");
        menu->addChild(m_albumCoverSprite);

        this->setPlayback();
        this->schedule(schedule_selector(Playhead::setPlayback), 2.f);
        
        this->m_isPauseButton = true;
    }

    this->setID("dashify-playhead");
    this->setZOrder(3);

    return true;
}

void Playhead::setPlayback() {
    m_playbackListener.bind([=, this] (web::WebTask::Event* e) {
        if (web::WebResponse* res = e->getValue()) {
            if (res->code() == 429) {
                Notification::create("Dashify Error: Too Many Requests", NotificationIcon::Error)->show();
            } else if (res->ok()) {
                std::vector<Artist*> artists;

                auto jsonRes = res->json().unwrap();
                auto isPlaying = jsonRes["is_playing"].as_bool();
                auto albumCoverURL = jsonRes["item"]["album"]["images"][0]["url"].as_string();
                auto albumName = jsonRes["item"]["album"]["name"].as_string();
                auto songName = jsonRes["item"]["name"].as_string();

                auto artistsJSON = jsonRes["item"]["artists"].as_array();
                for (auto artist : artistsJSON) {
                    artists.push_back(new Artist(artist["name"].as_string()));
                }

                m_imageDataListener.bind([=, this] (web::WebTask::Event* e) {
                    if (web::WebResponse* res = e->getValue()) {
                        if (res->ok()) {
                            auto imageData = res->data();
                            CCImage* image = new CCImage();
                            image->initWithImageData(const_cast<uint8_t*>(imageData.data()), imageData.size());
                            
                            CCTexture2D* texture = new CCTexture2D();
                            texture->initWithImage(image);

                            this->m_albumCoverSprite->initWithTexture(texture);
                            this->m_albumCoverSprite->setScale(0.35f);

                            image->release();
                            texture->release();
                        }
                    } else if (e->isCancelled()) {
                        log::error("Error with image data");
                    }
                });

                auto playButtonSprite = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
                playButtonSprite->setScale(0.5f);

                this->m_playControlButton->setSprite((isPlaying) ? CCSprite::createWithSpriteFrameName("GJ_pauseEditorBtn_001.png") : playButtonSprite);

                auto imageReq = web::WebRequest();
                m_imageDataListener.setFilter(imageReq.get(albumCoverURL));

                this->m_albumTitleLabel->setString(fmt::format("On {}", albumName).c_str());
                this->m_songTitleLabel->setString(songName.c_str());

                std::string artistsLabelText = "By ";
                for (int i = 0; i < artists.size(); i++) {
                    if (artists[i] != artists.back()) {
                        artistsLabelText += artists[i]->name + ", ";
                    } else {
                        artistsLabelText += artists[i]->name;
                    }
                }
                this->m_songArtistsLabel->setString(artistsLabelText.c_str());
            }
        } else if (e->isCancelled()) {
            log::error("playback request was cancelled");
        }
    });

    auto req = web::WebRequest();
    req.header("Authorization", fmt::format("Bearer {}", this->spotify->getToken()));
    m_playbackListener.setFilter(req.get("https://api.spotify.com/v1/me/player"));
}

void Playhead::onLogin(CCObject* sender) {
    auto senderNode = static_cast<CCMenuItemSpriteExtra*>(sender);
    senderNode->setScale(0.65f);

    geode::createQuickPopup("Login?", fmt::format("Login? This will take you to <cg>Spotify's</c> login page."), "Cancel", "Ok", [=, this](auto, bool btn2) {
        if (btn2) {
            web::openLinkInBrowser("http://localhost:18080/login");
        }
    });
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

void Playhead::onSkip(CCObject* sender) {
    auto senderNode = static_cast<CCMenuItemSpriteExtra*>(sender);
    senderNode->setScale(0.6f);

    this->spotify->skipSong();
}

void Playhead::onPrevious(CCObject* sender) {
    auto senderNode = static_cast<CCMenuItemSpriteExtra*>(sender);
    senderNode->setScale(0.6f);

    this->spotify->previousSong();
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