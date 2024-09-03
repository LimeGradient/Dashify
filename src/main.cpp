#include <Geode/Geode.hpp>
#include "Spotify/Spotify.hpp"
#include "UI/Playhead.hpp"

using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>

Spotify* m_spotify;
bool showPlayhead = true;

$on_mod(Loaded) {
	m_spotify = new Spotify();
	m_spotify->init();
}

class $modify(DashifyMenuLayer, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) {
			return false;
		}

		auto director = CCDirector::get();

		if (!CCScene::get()->getChildByID("dashify-playhead") && Mod::get()->getSettingValue<bool>("show-only-editor") == false) {
			auto playheadMenu = Playhead::create(m_spotify);
			playheadMenu->setPosition({0.f, 0.f});
			
			this->addChild(playheadMenu);
			SceneManager::get()->keepAcrossScenes(playheadMenu);
		}

		if (!CCScene::get()->getChildByID("dashify-button-menu") && Mod::get()->getSettingValue<bool>("show-dashify-button")) {
			auto menu = CCMenu::create();
			menu->setID("dashify-button-menu");
			menu->setPosition({0.f, 0.f});
			this->addChild(menu);
			SceneManager::get()->keepAcrossScenes(menu);

			auto dashifyButtonSprite = CCSprite::createWithSpriteFrameName("dashify-button.png"_spr);
			dashifyButtonSprite->setScale(.65f);

			auto dashifyPlayheadButton = CCMenuItemSpriteExtra::create(
				dashifyButtonSprite,
				this,
				menu_selector(DashifyMenuLayer::onDashifyButton)
			);
			dashifyPlayheadButton->setPosition({director->getScreenRight() - 19.f, director->getScreenTop() - 100.f});
			dashifyPlayheadButton->setID("dashify-button");
			menu->addChild(dashifyPlayheadButton);
		}

		return true;
	}

	void onDashifyButton(CCObject* sender) {
		if (CCScene::get()->getChildByID("dashify-playhead")) {
			auto playheadNode = CCScene::get()->getChildByID("dashify-playhead");

			showPlayhead = !showPlayhead;
			playheadNode->setVisible(showPlayhead);
		}
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

class $modify(PlayLayer) {
	bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		if (!PlayLayer::init(level, useReplay, dontCreateObjects)) {
			return false;
		}

		if (!Mod::get()->getSettingValue<bool>("show-in-level")) {
			CCScene::get()->getChildByID("dashify-playhead")->setVisible(false);
		}

		return true;
	}

	void onQuit() {
		if (!Mod::get()->getSettingValue<bool>("show-in-level")) {
			CCScene::get()->getChildByID("dashify-playhead")->setVisible(true);
		}

		PlayLayer::onQuit();
	}
};

// thank you firee for pushing me in the right direction
class $modify(CCKeyboardDispatcher) {
	bool dispatchKeyboardMSG(enumKeyCodes key, bool down, bool arr) {
		if (down) {
			if (CCScene::get() == nullptr) return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, arr);
			if (key == KEY_P) {
				if (CCScene::get()->getChildByID("dashify-playhead")) {
					auto playheadNode = CCScene::get()->getChildByID("dashify-playhead");

					showPlayhead = !showPlayhead;
					playheadNode->setVisible(showPlayhead);
				}
			}
		}
		return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, arr);
	}
};