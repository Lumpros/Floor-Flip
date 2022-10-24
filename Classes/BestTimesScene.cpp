//
// Created by Lumpros on 10/20/2022.
//

#include "BestTimesScene.h"
#include "BestTimeManager.h"

#include "ui/CocosGUI.h"

USING_NS_CC;

bool BestTimesScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    initLabels();
    initBackground();
    initBackButton();

    return true;
}

void BestTimesScene::initLabels()
{
    BestTimeManager* pManager = BestTimeManager::getInstance();

    const Vec2 ptOrigin = Director::getInstance()->getVisibleOrigin();
    const Size visibleSize = Director::getInstance()->getVisibleSize();

    Label* pModesLabel = Label::createWithTTF("4x4:\n5x5:\n7x7:\n9x9:\n12x12:", "fonts/Roboto-Light.ttf", 16);

    if (pModesLabel)
    {
        pModesLabel->setPosition(ptOrigin + Vec2(pModesLabel->getContentSize().width / 2 + 5, visibleSize.height / 2));

        addChild(pModesLabel);
    }

    const char* records = "%s\n%s\n%s\n%s\n%s";

    char buffer[256];
    sprintf(buffer, records,
            pManager->getBestTime("4x4").c_str(),
            pManager->getBestTime("5x5").c_str(),
            pManager->getBestTime("7x7").c_str(),
            pManager->getBestTime("9x9").c_str(),
            pManager->getBestTime("12x12").c_str());

    Label* pRecordLabel = Label::createWithTTF(buffer, "fonts/Roboto-Light.ttf", 16);

    if (pRecordLabel)
    {
        pRecordLabel->setTextColor(Color4B(200, 200, 200, 255));
        pRecordLabel->setPosition(ptOrigin + Vec2(visibleSize.width - pRecordLabel->getContentSize().width / 2 - 5, visibleSize.height / 2));

        addChild(pRecordLabel);
    }

    Label* pBestTimesLabel = Label::createWithTTF("Best Times", "fonts/Roboto-Light.ttf", 18);

    if (pBestTimesLabel)
    {
        pBestTimesLabel->setPosition(ptOrigin + Vec2(visibleSize.width / 2, visibleSize.height * 12 / 16));

        addChild(pBestTimesLabel);
    }
}

void BestTimesScene::initBackground()
{
    const Color4B bgColor = Color4B(30, 30, 30, 255);

    LayerColor* pBackgroundLayer = LayerColor::create(bgColor);

    if (pBackgroundLayer)
    {
        this->addChild(pBackgroundLayer, -10);
    }
}

void BestTimesScene::initBackButton()
{
    ui::Button* pBackButton = ui::Button::create("ui/arrowLeft.png");

    if (pBackButton)
    {
        Director* pDirector = Director::getInstance();
        const Vec2 ptOrigin = pDirector->getVisibleOrigin();
        const Size visibleSize = pDirector->getVisibleSize();

        pBackButton->setPosition(Vec2(ptOrigin.x + pBackButton->getContentSize().width / 2,
                                      ptOrigin.y + visibleSize.height - pBackButton->getContentSize().height / 2));

        pBackButton->addTouchEventListener([&](Ref* pSender, ui::Widget::TouchEventType type) {
            if (type == ui::Widget::TouchEventType::ENDED) {
                Director::getInstance()->popScene();
            }
        });

        addChild(pBackButton);
    }
}