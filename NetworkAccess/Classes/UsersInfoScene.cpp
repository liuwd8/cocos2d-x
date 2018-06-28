#include "UsersInfoScene.h"

cocos2d::Scene * UsersInfoScene::createScene() {
  return UsersInfoScene::create();
}

bool UsersInfoScene::init() {
  if (!Scene::init()) return false;

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto getUserButton = MenuItemFont::create("Get User", CC_CALLBACK_1(UsersInfoScene::getUserButtonCallback, this));
  if (getUserButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + getUserButton->getContentSize().height / 2;
    getUserButton->setPosition(Vec2(x, y));
  }

  auto backButton = MenuItemFont::create("Back", [](Ref* pSender) {
    Director::getInstance()->popScene();
  });
  if (backButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
    backButton->setPosition(Vec2(x, y));
  }

  auto menu = Menu::create(getUserButton, backButton, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  limitInput = TextField::create("limit", "arial", 24);
  if (limitInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 100.0f;
    limitInput->setPosition(Vec2(x, y));
    this->addChild(limitInput, 1);
  }

  messageBox = Label::create("", "arial", 30);
  if (messageBox) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height / 2;
    messageBox->setPosition(Vec2(x, y));
    this->addChild(messageBox, 1);
  }

  return true;
}

void UsersInfoScene::onHttpGetComplete(HttpClient *sender, HttpResponse *response) {
	if (!response) return;

	if (!response->isSucceed()) {
		log("Respone failed");
		log("Error buffer: %s", response->getErrorBuffer());
		return;
	}

	std::vector<char> *buffer = response->getResponseData();
	rapidjson::Document doc;
	doc.Parse(buffer->data(), buffer->size());

	if (doc["status"] == true) {
		std::string str("");
		rapidjson::Value array(doc["data"].GetArray());
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		array.Accept(writer);
		log("%s", buffer.GetString());

		for (auto i = 0; i < array.Size(); ++i) {
			buffer.Clear();
			writer.Reset(buffer);
			if(array[i]["deck"].Size() > 0) array[i]["deck"].Accept(writer);
			str += std::string("Username: ") + array[i]["username"].GetString() + "\nDeck: " + buffer.GetString() + "\n---\n";
		}

		messageBox->setString(str);
	}
	else messageBox->setString(std::string("Register failed.\n") + doc["msg"].GetString());
}

void UsersInfoScene::getUserButtonCallback(Ref * pSender) {
  // Your code here
	if (std::regex_match(limitInput->getString(), std::regex("[0-9]+"))) {
		HttpRequest* request = new HttpRequest();
		request->setRequestType(HttpRequest::Type::GET);
		request->setUrl(std::string("http://127.0.0.1:8000/users?limit=") + limitInput->getString());
		request->setResponseCallback(CC_CALLBACK_2(UsersInfoScene::onHttpGetComplete, this));

		HttpClient::getInstance()->send(request);
		request->release();
	}
}
