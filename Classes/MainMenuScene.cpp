#include "MainMenuScene.h"
#include "GameScene.h"

USING_NS_CC;

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#define DO_IF_VALID(x, y) if (x) { (x)->y; }

static const char* g_modes[] = {
        "Tiny - 4x4",
        "Small - 5x5",
        "Medium - 7x7",
        "Big - 9x9",
        "Huge - 12x12"
};

static cocos2d::Sprite* g_modeSprites[] = {
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
};

Scene* MainMenu::createScene()
{
    return MainMenu::create();
}

MainMenu::~MainMenu()
{
    for (uint8_t i = 0; i < ARRAY_SIZE(g_modeSprites); ++i)
    {
        g_modeSprites[i]->release();
    }
}

bool MainMenu::init()
{
    if (!Scene::init())
    {
        return false;
    }

    SpriteFrameCache* pCache = SpriteFrameCache::getInstance();
    pCache->addSpriteFramesWithFile("tiles/tilesheet.plist");

    initPictures();
    initHiddenLabels();
    initBackground();
    initCocosMenu();
    initModeSelector();
    initInfoButton();

    return true;
}

void MainMenu::initPictures()
{
    g_modeSprites[0] = Sprite::create("tiles/4x4.png");
    g_modeSprites[1] = Sprite::create("tiles/5x5.png");
    g_modeSprites[2] = Sprite::create("tiles/7x7.png");
    g_modeSprites[3] = Sprite::create("tiles/9x9.png");
    g_modeSprites[4] = Sprite::create("tiles/11x11.png");

    for (uint8_t i = 0; i < ARRAY_SIZE(g_modeSprites); ++i)
    {
        DO_IF_VALID(g_modeSprites[i], retain());
        addChild(g_modeSprites[i]);
    }
}

void MainMenu::initHiddenLabels()
{
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();

    for (size_t i = 0; i < ARRAY_SIZE(g_modes); ++i)
    {
        Label* pLabel = Label::createWithTTF(g_modes[i], "fonts/Roboto-Light.ttf", 12);

        if (pLabel)
        {
            addChild(pLabel);

            pLabel->setOpacity(0);
            pLabel->setTextColor(Color4B(0xFF, 0xFF, 0xFF, 255));
            pLabel->setPositionY(visibleSize.height * 8 / 16 + origin.y);
            pLabel->setPositionX(visibleSize.width / 2 + origin.x);

            modeLabels.pushBack(pLabel);
        }
    }
}

void MainMenu::initBackground()
{
    const Color4B bgColor = Color4B(30, 30, 30, 255);

    LayerColor* pBackgroundLayer = LayerColor::create(bgColor);

    if (pBackgroundLayer)
    {
        this->addChild(pBackgroundLayer, -10);
    }

    // Add these layers to the left and to the right of the screen
    // so when the user presses the left or the right button and we
    // scroll the content, it disappears behind these layers instead
    // of being visible all the way across the screen.
    LayerColor* pLeftLayer = LayerColor::create(bgColor);
    LayerColor* pRightLayer = LayerColor::create(bgColor);

    const Vec2 ptOrigin = Director::getInstance()->getVisibleOrigin();
    const Size visibleSize = Director::getInstance()->getVisibleSize();

    if (pLeftLayer)
    {
        pLeftLayer->setPosition(Vec2(ptOrigin.x - 5,  ptOrigin.y));
        pLeftLayer->setContentSize(Size(visibleSize.width / 5 + 5, visibleSize.height));

        this->addChild(pLeftLayer, 2);
    }

    if (pRightLayer)
    {
        pRightLayer->setPosition(Vec2(ptOrigin.x + visibleSize.width * 4 / 5, ptOrigin.y));
        pRightLayer->setContentSize(Size(visibleSize.width / 5, visibleSize.height));

        this->addChild(pRightLayer, 2);
    }
}

void MainMenu::initCocosMenu()
{
    const Size visibleSize = Director::getInstance()->getVisibleSize();

    Vector<MenuItem*> menuItems;
    menuItems.pushBack(MenuItemImage::create("menu/free-play-unselected.png", "menu/free-play-selected.png",
                                             CC_CALLBACK_1(MainMenu::onPressPlay, this)));
    menuItems.pushBack(
            MenuItemImage::create("menu/times-unselected.png", "menu/times-selected.png",
                                  CC_CALLBACK_1(MainMenu::onPressTimes, this)));

    pMenu = Menu::createWithArray(menuItems);

    if (pMenu)
    {
        pMenu->alignItemsVerticallyWithPadding(0.0);
        pMenu->setPositionY(visibleSize.height * 5 / 16);

        if (std::find(menuItems.begin(), menuItems.end(), nullptr) == menuItems.end()) {
            this->addChild(pMenu, 3);
        }
    }
}

void MainMenu::onPressPlay(Ref* pSender)
{
    const std::string& label_string = pModeLabel->getString();

    size_t xIndex = label_string.size() - 1;
    for (; xIndex >= 0 && label_string[xIndex] != 'x'; --xIndex) {}

    if (xIndex != -1)
    {
        std::string size = label_string.substr(xIndex + 1, label_string.size() - 1);

        uint8_t boardSize = atoi(size.c_str());

        GameScene *pGameScene = GameScene::create();

        if (pGameScene)
        {
            pGameScene->initBoard(boardSize, boardSize);
            Director::getInstance()->pushScene(TransitionCrossFade::create(0.4, pGameScene));
        }
    }
}

void MainMenu::onPressTimes(Ref* pSender)
{

}

void MainMenu::initInfoButton()
{
    ui::Button* pInfoButton = ui::Button::create("menu/information.png");

    if (pInfoButton)
    {
        const Size visibleSize = Director::getInstance()->getVisibleSize();

        pInfoButton->setPosition(
                Director::getInstance()->getVisibleOrigin()
                - Vec2(pInfoButton->getContentSize().width / 2, pInfoButton->getContentSize().height / 2)
                + Vec2(visibleSize.width, visibleSize.height)
        );

        addChild(pInfoButton, 5);
    }
}

void MainMenu::initModeSelector()
{
    pScrollLeftButton = ui::Button::create("menu/backward.png");
    pScrollRightButton = ui::Button::create("menu/forward.png");

    const Vec2 ptOrigin = Director::getInstance()->getVisibleOrigin();
    const Size visibleSize = Director::getInstance()->getVisibleSize();

    if (pScrollLeftButton)
    {
        pScrollLeftButton->addTouchEventListener(CC_CALLBACK_2(MainMenu::onPressLeft, this));
        pScrollLeftButton->setPosition(ptOrigin + Vec2(visibleSize.width / 8, visibleSize.height * 8 / 16));

        this->addChild(pScrollLeftButton, 2);
    }

    if (pScrollRightButton)
    {
        pScrollRightButton->addTouchEventListener(CC_CALLBACK_2(MainMenu::onPressRight, this));
        pScrollRightButton->setPosition(ptOrigin + Vec2(visibleSize.width * 7 / 8, visibleSize.height * 8 / 16));

        this->addChild(pScrollRightButton, 2);
    }

    pModeLabel = modeLabels.at(iModeIndex);
    DO_IF_VALID(pModeLabel, setOpacity(0xFF));

    Point origin = Director::getInstance()->getVisibleOrigin();

    for (size_t i = 0; i < ARRAY_SIZE(g_modeSprites); ++i)
    {
        if (g_modeSprites[i])
        {
            g_modeSprites[i]->setOpacity(0);
            g_modeSprites[i]->setContentSize(Size(visibleSize.width / 2, visibleSize.width / 2));

            g_modeSprites[i]->setPositionY(visibleSize.height * 11 / 16 + origin.y);
            g_modeSprites[i]->setPositionX(visibleSize.width / 2 + origin.x);
        }
    }

    pModeSprite = g_modeSprites[iModeIndex];
    DO_IF_VALID(pModeSprite, setOpacity(0xFF));
}

void MainMenu::onPressLeft(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 ptOrigin = Director::getInstance()->getVisibleOrigin();

    switch (type)
    {
        case ui::Widget::TouchEventType::BEGAN:
            break;
        case ui::Widget::TouchEventType::ENDED:
            pPrevLabel = pModeLabel;
            pPrevSprite = pModeSprite;
            if (iModeIndex == 0)
                iModeIndex = ARRAY_SIZE(g_modes) - 1;
            else
                --iModeIndex;
            pModeLabel = modeLabels.at(iModeIndex);
            pModeSprite = g_modeSprites[iModeIndex];
            pModeLabel->stopAllActions();
            pPrevLabel->stopAllActions();
            pModeSprite->stopAllActions();
            pPrevSprite->stopAllActions();
            pModeLabel->setPositionX(visibleSize.width / 2 - visibleSize.width + ptOrigin.x);
            pModeSprite->setPositionX(visibleSize.width / 2 - visibleSize.width + ptOrigin.x);
            shiftToRight(pPrevLabel, ptOrigin, visibleSize);
            shiftToRight(pPrevSprite, ptOrigin, visibleSize);
            shiftToCenter(pModeLabel, ptOrigin, visibleSize);
            shiftToCenter(pModeSprite, ptOrigin, visibleSize);
            break;
        default:
            break;
    }
}

void MainMenu::onPressRight(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 ptOrigin = Director::getInstance()->getVisibleOrigin();

    switch (type)
    {
        case ui::Widget::TouchEventType::BEGAN:
            break;
        case ui::Widget::TouchEventType::ENDED:
            pPrevLabel = pModeLabel;
            pPrevSprite = pModeSprite;
            iModeIndex = (iModeIndex + 1) % ARRAY_SIZE(g_modes);
            pModeLabel = modeLabels.at(iModeIndex);
            pModeSprite = g_modeSprites[iModeIndex];
            pModeLabel->stopAllActions();
            pPrevLabel->stopAllActions();
            pModeSprite->stopAllActions();
            pPrevSprite->stopAllActions();
            pModeLabel->setPositionX(visibleSize.width / 2 + visibleSize.width + ptOrigin.x);
            pModeSprite->setPositionX(visibleSize.width / 2 + visibleSize.width + ptOrigin.x);
            shiftToLeft(pPrevLabel, ptOrigin, visibleSize);
            shiftToLeft(pPrevSprite, ptOrigin, visibleSize);
            shiftToCenter(pModeLabel, ptOrigin, visibleSize);
            shiftToCenter(pModeSprite, ptOrigin, visibleSize);
            break;
        default:
            break;
    }
}

void MainMenu::shiftToCenter(cocos2d::Node* pNode, const cocos2d::Vec2& ptOrigin, const cocos2d::Size& visibleSize)
{
    assert(pNode);

    MoveTo* pMoveToCenterAction = MoveTo::create(0.25, Vec2(visibleSize.width / 2 + ptOrigin.x, pNode->getPositionY()));

    pNode->runAction(Sequence::create(FadeTo::create(0, 255), pMoveToCenterAction, nullptr));
}

void MainMenu::shiftToLeft(cocos2d::Node* pNode, const cocos2d::Vec2& ptOrigin, const cocos2d::Size& visibleSize)
{
    assert(pNode);

    MoveTo* pMoveToLeftAction = MoveTo::create(0.25, Vec2(visibleSize.width / 2 - visibleSize.width + ptOrigin.x, pNode->getPositionY()));

    pNode->runAction(Sequence::create(pMoveToLeftAction, FadeTo::create(0.0f, 0.0f), nullptr));
}

void MainMenu::shiftToRight(cocos2d::Node* pNode, const cocos2d::Vec2& ptOrigin, const cocos2d::Size& visibleSize)
{
    assert(pNode);

    MoveTo* pMoveToRightAction = MoveTo::create(0.25, Vec2(visibleSize.width * 1.5 + ptOrigin.x, pNode->getPositionY()));

    pNode->runAction(Sequence::create(pMoveToRightAction, FadeTo::create(0, 0), nullptr));
}