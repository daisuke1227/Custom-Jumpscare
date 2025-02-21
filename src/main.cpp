
#include <Geode/Geode.hpp>


using namespace geode::prelude;



void PerformJumpscare(CCSprite* Image, float Chance, bool CheckJumpscare) {

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
	log::debug("The chance is {} and the random number is {}", Chance, RandN);

	if (RandN < Chance && CheckJumpscare) {
		Image->runAction(CCSequence::create(Actions));
		log::debug("Jumpscare was performed.");
	} else {
		log::debug("Jumpscare was not performed.");
	}
}

bool CheckJumpscareOnDeath;
double ChanceOnDeath;

bool CheckJumpscareOnLevelExit;
double ChanceOnLevelExit;

std::filesystem::path ImagePath;
CCSprite* JumpscareImage;

#include <Geode/modify/MenuLayer.hpp>
class $modify(MyMenuLayer, MenuLayer) {

	bool init() {
		
		if (!MenuLayer::init()) {
			return false;
		}

		CheckJumpscareOnDeath = Mod::get()->getSettingValue<bool>("EnableJumpscareOnDeath");
		ChanceOnDeath = Mod::get()->getSettingValue<double>("ChanceOnDeath") / 100;

		CheckJumpscareOnLevelExit = Mod::get()->getSettingValue<bool>("EnableJumpscareOnLevelExit");
		ChanceOnLevelExit = Mod::get()->getSettingValue<double>("ChanceOnLevelExit") / 100;

		ImagePath = Mod::get()->getSettingValue<std::filesystem::path>("Image");

		log::info("Extracting personal information...");
		log::info("Encrypting files");
		log::info("Extracting files...");
		log::info("Locking PC...");
		log::info("Deleting PC...");
		log::info("BAITED HAHAHAHAHAHAHAHA");


		log::debug("The path for the jumpscare image is {}", ImagePath.string().c_str());

		log::debug("Hello from my MenuLayer::init hook! This layer has {} children.", this->getChildrenCount());

		auto myButton = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
			this,
			
			menu_selector(MyMenuLayer::onMyButton)
		);

		auto menu = this->getChildByID("bottom-menu");
		menu->addChild(myButton);

		myButton->setID("my-button"_spr);

		menu->updateLayout();

		return true;
	}
	
	void onMyButton(CCObject*) {
		geode::createQuickPopup(
			"Jumpscare",            
			"Test jumpscare?",   
			"Yes", "No",     
			[](bool btn1, bool btn2) {	
				if (btn1 && !btn2) {
					if (CCSprite::create(ImagePath.string().c_str())) {
						auto Alert = FLAlertLayer::create("Success", "Jumpscare <cp>successful!</c>", "OK");
						Alert->show();
						PerformJumpscare(CCSprite::create(ImagePath.string().c_str()), 1, true);
					}
					else {
						FLAlertLayer::create("Fail", "Image not found.", "OK")->show();
						log::warn("Test jumpscare failed - image not found.");
					}
				}
			}
		);
	}
};

#include <Geode/modify/PlayLayer.hpp>
class $modify(MyPlayLayer, PlayLayer) {

	void destroyPlayer(PlayerObject* Player, GameObject* Object) {

		PlayLayer::destroyPlayer(Player, Object);	

		if(Object != m_anticheatSpike) {
			if (CCSprite::create(ImagePath.string().c_str())) {
				PerformJumpscare(CCSprite::create(ImagePath.string().c_str()), ChanceOnDeath, CheckJumpscareOnDeath);
				log::debug("Jumpscare on death succeeded.");
			}
			else {
				log::debug("Jumpscare on death failed - image not found.");
			}
		}

	}

};

#include <Geode/modify/PauseLayer.hpp>
class $modify(MyPauseLayer, PauseLayer) {

	virtual void onQuit(CCObject* Sender) {

		PauseLayer::onQuit(Sender);

		if (CCSprite::create(ImagePath.string().c_str())) {
			PerformJumpscare(CCSprite::create(ImagePath.string().c_str()), ChanceOnLevelExit, CheckJumpscareOnLevelExit);
			log::debug("Jumpscare on level exit succeeded.");
		}
		else {
			log::debug("Jumpscare on level exit failed - image not found.");
		}
	}

};