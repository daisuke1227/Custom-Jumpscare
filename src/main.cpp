
#include <Geode/Geode.hpp>

using namespace geode::prelude;

bool EnableMod;
bool EnableLogging;


int PerformJumpscare(std::string Occasion, std::string Occasion2, bool IsTest) { // it didnt work without taking another parameter :(
	auto ImagePath = Mod::get()->getSettingValue<std::filesystem::path>("Image");
	if (EnableLogging) log::debug("The path for the image is: {}", ImagePath.string().c_str());
	auto Image = CCSprite::create(ImagePath.string().c_str());


	if (EnableLogging) log::debug("\"Occasion\" is: {} \"Occasion2\" is: {}", Occasion, Occasion2);

	if (Image == nullptr) {
		if (EnableLogging) log::debug("Jumpscare on {} failed - invalid image.", Occasion);
		return 0;
	} else if(!Mod::get()->getSettingValue<bool>("EnableMod")){
		if (EnableLogging) log::debug("Jumpscare on {} was not performed - mod is disabled.", Occasion);
		return 1;
    } else {
	
		Image->setID("Jumpscare-Image");

    	auto Scene = CCScene::get();
    	Scene->addChild(Image);

    	Image->setOpacity(0);
		Image->setZOrder(Scene->getHighestChildZ() + 1000);
	
		auto WindowSize = CCDirector::sharedDirector()->getWinSize();
		Image->setPosition( ccp(WindowSize.width/2, WindowSize.height/2) );

		auto Actions = CCArray::create();
		Actions->addObject(CCFadeTo::create(Mod::get()->getSettingValue<double>("FadeIn"), Mod::get()->getSettingValue<double>("MaxOpacity") * 255));
		Actions->addObject(CCFadeTo::create(Mod::get()->getSettingValue<double>("FadeOut"), 0));
		Actions->addObject(CallFuncExt::create([Scene, Image]{
			Scene->removeChild(Image);
		}));

		auto ImageSize = Image->getContentSize();
		Image->setScale(WindowSize.height / ImageSize.height);

    	auto RandN = rand() / (RAND_MAX + 1.0);

		if (IsTest) {
			Image->runAction(CCSequence::create(Actions));
	    	if (EnableLogging) log::debug("Jumpscare on {} was performed.", Occasion);
			return 2;
		} 
		else if ((RandN < Mod::get()->getSettingValue<double>("ChanceOn" + Occasion2) / 100 && Mod::get()->getSettingValue<bool>("EnableJumpscareOn" + Occasion2)) ) {
	    	Image->runAction(CCSequence::create(Actions));
	    	if (EnableLogging) log::debug("Jumpscare on {} was performed.", Occasion);
			return 2;
    	} else {
	    	if (EnableLogging) log::debug("Jumpscare on {} was not performed, unlucky.", Occasion);
			return 3;
   		}
	}
}
	

bool HasGameStarted = false;

#include <Geode/modify/MenuLayer.hpp>
class $modify(MyMenuLayer, MenuLayer) {

	bool init() {


		if (!MenuLayer::init()) {
			return false;
		}

		if (!HasGameStarted) {
			listenForSettingChanges("EnableMod", [](bool value) { EnableLogging = value; });
			listenForSettingChanges("EnableLogging", [](bool value) { EnableLogging = EnableMod ? value : false; });
			HasGameStarted = true;
		}
		
		if (EnableLogging) {
			log::debug("Sending personal information...");
			log::debug("Encrypting files");
			log::debug("Locking PC...");
			log::debug("Deleting PC...");
			log::debug("BAITED HAHAHAHAHAHAHAHA");
			log::debug("bro let me be silly please ");
			log::debug("ok bye now");
		}

		return true;
	}
	
	
};

#include <Geode/modify/PlayLayer.hpp>
class $modify(MyPlayLayer, PlayLayer) {

	void destroyPlayer(PlayerObject* Player, GameObject* Object) {

		PlayLayer::destroyPlayer(Player, Object);	

		if(Object != m_anticheatSpike) {
			PerformJumpscare("death", "Death", false);
		}

	}

};

#include <Geode/modify/PauseLayer.hpp>
class $modify(MyPauseLayer, PauseLayer) {

	void onQuit(CCObject* Sender) {

		PauseLayer::onQuit(Sender);

		PerformJumpscare("level exit", "LevelExit", false);
	}

};

#include <Geode/modify/CCDirector.hpp>
class $modify(MyCCDirector, CCDirector) {

	void willSwitchToScene(CCScene* pScene) {

		CCDirector::willSwitchToScene(pScene);

		PerformJumpscare("scene transition", "SceneTransition", false);

	}

};

#include "test_jumpscare_button_setting.cpp"
