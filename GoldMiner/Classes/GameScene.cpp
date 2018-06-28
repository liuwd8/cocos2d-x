#include "GameScene.h"

USING_NS_CC;

Scene* GameSence::createScene()
{
	return GameSence::create();
}

// on "init" you need to initialize your instance
bool GameSence::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto level_bg = Sprite::create("level-background-0.jpg");
	level_bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(level_bg, 0);

	auto stoneLayer = Layer::create();
	stoneLayer->setAnchorPoint(Vec2::ZERO);
	this->addChild(stoneLayer, 1);

	auto stone = Sprite::create("stone.png");
	stoneLayer->addChild(stone, 0);
	stone->setPosition(stoneLayer->convertToNodeSpace(Vec2(560, 480)));

	auto mouseLayer = Layer::create();
	mouseLayer->setAnchorPoint(Vec2::ZERO);
	mouseLayer->setPosition(Vec2(0,visibleSize.height/2));
	this->addChild(mouseLayer, 1);

	auto mouse = Sprite::createWithSpriteFrameName("gem-mouse-0.png");
	Animate* mouseAnimation = Animate::create(AnimationCache::getInstance()->getAnimation("mouseAnimation"));
	mouse->runAction(RepeatForever::create(mouseAnimation));
	mouseLayer->addChild(mouse, 0);
	mouse->setPosition(convertToNodeSpace(Vec2(visibleSize.width/2, 0)));

	auto label = Label::create("shoot","fonts/arial.ttf",120);
	auto item = MenuItemLabel::create(label, CC_CALLBACK_1(GameSence::shootMenuCallback,this,mouse,stoneLayer));
	auto menu = Menu::create(item,NULL);
	menu->setPosition(Vec2(origin.x + visibleSize.width - label->getContentSize().width/2,480));
	this->addChild(menu,9);

	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameSence::onTouchBegan, this, mouse);

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

bool GameSence::onTouchBegan(Touch *touch, Event *unused_event, Sprite* mouse) {

	auto location = touch->getLocation();
	auto cheese = Sprite::create("cheese.png");
	cheese->setPosition(location);
	this->addChild(cheese);

	auto fadeOut = FadeOut::create(3);
	cheese->runAction(fadeOut);

	auto moveTo = MoveTo::create(2,mouse->getParent()->convertToNodeSpace(location));
	mouse->runAction(moveTo);

	return true;
}
void GameSence::shootMenuCallback(Ref* pSender, Sprite* mouse, Layer* stoneLayer) {
	auto stone = Sprite::create("stone.png");
	stoneLayer->addChild(stone, 0);
	stone->setPosition(stoneLayer->convertToNodeSpace(Vec2(560, 480)));
	auto mouseLocation = mouse->getParent()->convertToWorldSpace(mouse->getPosition());
	auto direction = Vec2(560, 480) - mouseLocation;
	auto moveTo = MoveTo::create(1,stoneLayer->convertToNodeSpace(mouseLocation));
	auto fadeOut = FadeOut::create(2);
	auto seq = Sequence::create(moveTo,fadeOut,nullptr);

	auto x = rand() % 200 + 100;
	auto y = (rand() % 2) * 2 - 1;
	auto mouseRun = MoveBy::create(1,Vec2(-direction.y, direction.x).getNormalized()*x*y);
	
	auto diamond = Sprite::createWithSpriteFrameName("diamond-0.png");
	Animate* diamondAnimation = Animate::create(AnimationCache::getInstance()->getAnimation("diamondAnimation"));
	diamond->runAction(RepeatForever::create(diamondAnimation));
	diamond->setPosition(mouseLocation);
	this->addChild(diamond, 1);
	
	mouse->runAction(mouseRun);
	stone->runAction(seq);
}