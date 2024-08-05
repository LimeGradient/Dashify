#include <Geode/Geode.hpp>
#include "Spotify/Spotify.hpp"

using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>
class $modify(MyMenuLayer, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) {
			return false;
		}

		log::debug("Hello from my MenuLayer::init hook! This layer has {} children.", this->getChildrenCount());

		Spotify* spotify = new Spotify();
		spotify->init();

		Playback* currentPlayback = spotify->getCurrentPlayback();
		log::info("song name: {}", currentPlayback->songName);

		return true;
	}

	void onMyButton(CCObject*) {
		FLAlertLayer::create("Geode", "Hello from my custom mod!", "OK")->show();
	}
};