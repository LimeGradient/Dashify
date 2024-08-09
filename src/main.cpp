#include <Geode/Geode.hpp>
#include "Spotify/Spotify.hpp"
#include "UI/Playhead.hpp"

using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>

Spotify* m_spotify;

$on_mod(Loaded) {
	m_spotify = new Spotify();
	m_spotify->init();
	m_spotify->m_isWebserverCreated = true;
}

class $modify(MyMenuLayer, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) {
			return false;
		}

		CCDirector* director = CCDirector::get();

/* 		Playback* currentPlayback = spotify->getCurrentPlayback();
		log::info("song name: {}", currentPlayback->songName); */

		auto loginButton = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("spotidash-button.png"_spr),
			this,
			menu_selector(MyMenuLayer::onLoginButton)
		);

		if (!CCScene::get()->getChildByID("dashify-playhead")) {
			auto playheadMenu = Playhead::create(m_spotify);
			playheadMenu->setPosition({0.f, 0.f});
			this->addChild(playheadMenu);
			SceneManager::get()->keepAcrossScenes(playheadMenu);
		}

		auto menu = this->getChildByID("bottom-menu");
		menu->addChild(loginButton);

		return true;
	}

	void onLoginButton(CCObject*) {
		geode::createQuickPopup("Authorize?", "This will redirect you to Spotify's authorization screen.", "Cancel", "Ok", [this](auto, bool btn2) {
			if (btn2) {
				web::openLinkInBrowser("http://localhost:18080/login");
			} else {
				m_spotify->pausePlayback();
			}
		});
	}
};