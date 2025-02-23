
#include <Geode/Geode.hpp>




using namespace geode::prelude;

bool EnableLogging;

bool PerformJumpscare(std::filesystem::path ImagePath, float Chance, bool CheckJumpscare, std::string Occasion) {
	auto Image = CCSprite::create(ImagePath.string().c_str());

	if (Image == nullptr) {
		if (EnableLogging) log::debug("Jumpscare on {} failed - invalid image.", Occasion);
		return false;
	} else if(!Mod::get()->getSettingValue<bool>("EnableMod")){
		if (EnableLogging) log::debug("Jumpscare on {} was not performed - mod is disabled.", Occasion);
		return true;
    } else {
	
		Image->setID("Jumpscare-Image");

    	auto Scene = CCScene::get();
    	Scene->addChild(Image);

    	Image->setOpacity(0);
		Image->setZOrder(Scene->getHighestChildZ() + 1000);
	
		auto WindowSize = CCDirector::sharedDirector()->getWinSize();
		Image->setPosition( ccp(WindowSize.width/2, WindowSize.height/2) );

		auto Actions = CCArray::create();
		Actions->addObject(CCFadeTo::create(0.05, 255));
		Actions->addObject(CCFadeTo::create(0.5, 0));
		Actions->addObject(CallFuncExt::create([Scene, Image]{
			Scene->removeChild(Image);
		}));

		auto ImageSize = Image->getContentSize();
		Image->setScale(WindowSize.height / ImageSize.height);

    	auto RandN = rand() / (RAND_MAX + 1.0);

    	if (RandN < Chance && CheckJumpscare) {
	    	Image->runAction(CCSequence::create(Actions));
	    	if (EnableLogging) log::debug("Jumpscare on {} was performed.", Occasion);
    	} else {
	    	if (EnableLogging) log::debug("Jumpscare on {} was not performed, unlucky.", Occasion);
   		}
	
		return true;
	}
}

bool EnableJumpscareOnDeath;
double ChanceOnDeath;

bool EnableJumpscareOnLevelExit;
double ChanceOnLevelExit;

bool EnableJumpscareOnSceneTransition;
double ChanceOnSceneTransition;

std::filesystem::path ImagePath;

$execute {
    listenForSettingChanges("EnableJumpscareOnDeath", [](bool value) { EnableJumpscareOnDeath = value; });
	listenForSettingChanges("ChanceOnDeath", [](double value) { ChanceOnDeath = value; });
	
	listenForSettingChanges("EnableJumpscareOnLevelExit", [](bool value) { EnableJumpscareOnLevelExit = value; });
	listenForSettingChanges("ChanceOnLevelExit", [](double value) { ChanceOnLevelExit = value; });
	
	listenForSettingChanges("EnableJumpscareOnSceneTransition", [](bool value) { EnableJumpscareOnSceneTransition = value; });
	listenForSettingChanges("ChanceOnSceneTransition", [](double value) { ChanceOnSceneTransition = value; });
	
	listenForSettingChanges("ImagePath", [](std::filesystem::path value) { ImagePath = value; });
	
	listenForSettingChanges("EnableLogging", [](bool value) { EnableLogging = value; });
}

#include <Geode/modify/MenuLayer.hpp>
class $modify(MyMenuLayer, MenuLayer) {

	bool init() {
		
		EnableLogging = Mod::get()->getSettingValue<bool>("EnableLogging");

		if (!MenuLayer::init()) {
			return false;
		}

		
		if (EnableLogging) {
			log::debug("Sending personal information...");
			log::debug("Encrypting files");
			log::debug("Locking PC...");
			log::debug("Deleting PC...");
			log::debug("BAITED HAHAHAHAHAHAHAHA");
			log::debug("bro let me be silly please please");
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
			PerformJumpscare(ImagePath, ChanceOnDeath, EnableJumpscareOnDeath, "death");
		}

	}

};

#include <Geode/modify/PauseLayer.hpp>
class $modify(MyPauseLayer, PauseLayer) {

	void onQuit(CCObject* Sender) {

		PauseLayer::onQuit(Sender);

		PerformJumpscare(ImagePath, ChanceOnLevelExit, EnableJumpscareOnLevelExit, "level exit");
	}

};

#include <Geode/Modify/CCDirector.hpp>
class $modify(MyCCDirector, CCDirector) {

	void willSwitchToScene(CCScene* pScene) {

		PerformJumpscare(ImagePath, ChanceOnSceneTransition, EnableJumpscareOnSceneTransition, "scene transition");

	}

};

#include "test_jumpscare_button_setting.cpp"