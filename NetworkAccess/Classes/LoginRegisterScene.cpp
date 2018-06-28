#include "LoginRegisterScene.h"

USING_NS_CC;

cocos2d::Scene * LoginRegisterScene::createScene() {
  return LoginRegisterScene::create();
}

bool LoginRegisterScene::init() {
  if (!Scene::init()) {
    return false;
  }

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto loginButton = MenuItemFont::create("Login", CC_CALLBACK_1(LoginRegisterScene::loginButtonCallback, this));
  if (loginButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + loginButton->getContentSize().height / 2;
    loginButton->setPosition(Vec2(x, y));
  }

  auto registerButton = MenuItemFont::create("Register", CC_CALLBACK_1(LoginRegisterScene::registerButtonCallback, this));
  if (registerButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + registerButton->getContentSize().height / 2 + 100;
    registerButton->setPosition(Vec2(x, y));
  }

  auto backButton = MenuItemFont::create("Back", [] (Ref* pSender) {
    Director::getInstance()->popScene();
  });
  if (backButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
    backButton->setPosition(Vec2(x, y));
  }

  auto menu = Menu::create(loginButton, registerButton, backButton, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  usernameInput = TextField::create("username", "arial", 24);
  if (usernameInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 100.0f;
    usernameInput->setPosition(Vec2(x, y));
    this->addChild(usernameInput, 1);
  }

  passwordInput = TextField::create("password", "arial", 24);
  if (passwordInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 130.0f;
    passwordInput->setPosition(Vec2(x, y));
    this->addChild(passwordInput, 1);
  }

  messageBox = Label::create("", "arial", 30);
  if (messageBox) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 200.0f;
    messageBox->setPosition(Vec2(x, y));
    this->addChild(messageBox, 1);
  }

  return true;
}

void LoginRegisterScene::loginButtonCallback(cocos2d::Ref * pSender) {
  // Your code here
	if (usernameInput->getStringLength() > 0 && passwordInput->getStringLength() > 0) {
		HttpRequest* request = new HttpRequest();
		request->setRequestType(HttpRequest::Type::POST);
		request->setUrl("http://127.0.0.1:8000/auth");
		request->setResponseCallback(CC_CALLBACK_2(LoginRegisterScene::onHttpLoginComplete, this));

		rapidjson::Document postData;
		postData.SetObject();
		rapidjson::Document::AllocatorType& allocator = postData.GetAllocator();
		postData.AddMember("username", rapidjson::StringRef(usernameInput->getString().c_str()), allocator);
		postData.AddMember("password", rapidjson::StringRef(passwordInput->getString().c_str()), allocator);

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		postData.Accept(writer);

		request->setRequestData(buffer.GetString(), buffer.GetSize());
		HttpClient::getInstance()->enableCookies(nullptr);
		HttpClient::getInstance()->send(request);
		request->release();
	}
	else {
		messageBox->setString("用户名或密码不能为空");
	}
}

void LoginRegisterScene::registerButtonCallback(Ref * pSender) {
  // Your code here
	if (usernameInput->getStringLength() > 0 && passwordInput->getStringLength() > 0) {
		HttpRequest* request = new HttpRequest();
		request->setRequestType(HttpRequest::Type::POST);
		request->setUrl("http://127.0.0.1:8000/users");
		request->setResponseCallback(CC_CALLBACK_2(LoginRegisterScene::onHttpRegisterComplete, this));

		rapidjson::Document postData;
		postData.SetObject();
		rapidjson::Document::AllocatorType& allocator = postData.GetAllocator();
		postData.AddMember("username", rapidjson::StringRef(usernameInput->getString().c_str()), allocator);
		postData.AddMember("password", rapidjson::StringRef(passwordInput->getString().c_str()), allocator);

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		postData.Accept(writer);

		request->setRequestData(buffer.GetString(), buffer.GetSize());
		HttpClient::getInstance()->send(request);
		request->release();
	}
	else {
		messageBox->setString("用户名或密码不能为空");
	}
}

void LoginRegisterScene::onHttpRegisterComplete(HttpClient * sender, HttpResponse * response)
{
	if (!response) return;

	if (!response->isSucceed()) {
		log("Respone failed");
		log("Error buffer: %s", response->getErrorBuffer());
		return;
	}

	std::vector<char> *buffer = response->getResponseData();
	rapidjson::Document doc;
	doc.Parse(buffer->data(), buffer->size());
	
	if (doc["status"] == true) messageBox->setString("Register OK.");
	else messageBox->setString(std::string("Register failed.\n") + doc["msg"].GetString());
}

void LoginRegisterScene::onHttpLoginComplete(HttpClient * sender, HttpResponse * response)
{
	if (!response) return;

	if (!response->isSucceed()) {
		log("Login failed");
		log("Error buffer: %s", response->getErrorBuffer());
		return;
	}

	std::vector<char> *buffer = response->getResponseData();
	rapidjson::Document doc;
	doc.Parse(buffer->data(), buffer->size());

	if (doc["status"] == true) messageBox->setString(doc["msg"].GetString());
	else messageBox->setString(std::string("Login failed.\n") + doc["msg"].GetString());
}
