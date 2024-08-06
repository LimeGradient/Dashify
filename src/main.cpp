#include <Geode/Geode.hpp>
#include "Spotify/Spotify.hpp"

using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>

class $modify(MyMenuLayer, MenuLayer) {
	
	bool init() {
		if (!MenuLayer::init()) {
			return false;
		}

		Spotify* spotify = new Spotify();
		spotify->m_isWebserverCreated = false;
		spotify->init();

/* 		Playback* currentPlayback = spotify->getCurrentPlayback();
		log::info("song name: {}", currentPlayback->songName); */

		log::debug("Hello from my MenuLayer::init hook! This layer has {} children.", this->getChildrenCount());

		return true;
	}

	void onMyButton(CCObject*) {
		FLAlertLayer::create("Geode", "Hello from my custom mod!", "OK")->show();
	}
};