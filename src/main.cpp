#include <Geode/Geode.hpp>
#include "Spotify/Spotify.hpp"
#include "UI/Playhead.hpp"

using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

#include <geode.custom-keybinds/include/Keybinds.hpp>

using namespace keybinds;

Spotify* m_spotify;
bool showPlayhead = true;

$on_mod(Loaded) {
	m_spotify = new Spotify();
	m_spotify->init();
}

$execute {
	BindManager::get()->registerBindable({
		"show-playhead"_spr,
		"Show Dashify Playhead",
		"Show/Hide the Dashify Playhead",
		{ Keybind::create(KEY_Q, Modifier::None) },
		"Dashify"
	});

	new EventListener([=](InvokeBindEvent* event) {
		if (event->isDown()) {
			if (CCScene::get()->getChildByID("dashify-playhead")) {
				auto playheadNode = static_cast<Playhead*>(CCScene::get()->getChildByID("dashify-playhead"));

				showPlayhead = !showPlayhead;
				playheadNode->setVisible(showPlayhead);
				log::info("show playhead: {}", showPlayhead);
			}
		}

		return ListenerResult::Propagate;
	}, InvokeBindFilter(nullptr, "show-playhead"_spr));
}

class $modify(MyMenuLayer, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) {
			return false;
		}

		if (!CCScene::get()->getChildByID("dashify-playhead") && Mod::get()->getSettingValue<bool>("show-only-editor") == false) {
			auto playheadMenu = Playhead::create(m_spotify);
			playheadMenu->setPosition({0.f, 0.f});
			this->addChild(playheadMenu);
			SceneManager::get()->keepAcrossScenes(playheadMenu);
		}

		return true;
	}
};

class $modify(LevelEditorLayer) {
	bool init(GJGameLevel* p0, bool p1) {
		if (!LevelEditorLayer::init(p0, p1)) {
			return false;
		}

		if (!CCScene::get()->getChildByID("dashify-playhead") && Mod::get()->getSettingValue<bool>("show-only-editor") == true) {
			auto playheadMenu = Playhead::create(m_spotify);
			playheadMenu->setPosition({0.f, 0.f});
			playheadMenu->setZOrder(101);
			this->addChild(playheadMenu);
		}

		return true;
	}
};