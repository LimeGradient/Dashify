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
		spotify->init();
		spotify->m_isWebserverCreated = true;

/* 		Playback* currentPlayback = spotify->getCurrentPlayback();
		log::info("song name: {}", currentPlayback->songName); */

		return true;
	}
};