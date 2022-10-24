#include "MainMenuScene.h"
#include "GameScene.h"
#include "BestTimesScene.h"

#include "firebase/gma.h"
#include "firebase/gma/types.h"

#include <thread>
#include <chrono>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include <platform/android/jni/JniHelper.h>
#endif

USING_NS_CC;

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef GET_ORIGIN_AND_SIZE
#define GET_ORIGIN_AND_SIZE(origin_name, size_name) \
        Director* pDirector = Director::getInstance(); \
        const Vec2 origin_name = pDirector->getVisibleOrigin(); \
        const Size size_name = pDirector->getVisibleSize();
#endif

#define DO_IF_VALID(x, y) if (x) { (x)->y; }

// Keep up to date with BestTimeManager
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
    initAd();

    return true;
}

void MainMenu::initPictures()
{
    GET_ORIGIN_AND_SIZE(ptOrigin, visibleSize);

    g_modeSprites[0] = Sprite::create("tiles/4x4.png");
    g_modeSprites[1] = Sprite::create("tiles/5x5.png");
    g_modeSprites[2] = Sprite::create("tiles/7x7.png");
    g_modeSprites[3] = Sprite::create("tiles/9x9.png");
    g_modeSprites[4] = Sprite::create("tiles/11x11.png");

    for (Sprite* pSprite : g_modeSprites)
    {
        if (pSprite)
        {
            pSprite->setOpacity(0);
            pSprite->setContentSize(Size(visibleSize.width / 2, visibleSize.width / 2));

            pSprite->setPositionY(visibleSize.height * 11 / 16 + ptOrigin.y);
            pSprite->setPositionX(visibleSize.width / 2 + ptOrigin.x);

            this->addChild(pSprite);
        }
    }
}

void MainMenu::initHiddenLabels()
{
    GET_ORIGIN_AND_SIZE(origin, visibleSize);

    for (const char* mode : g_modes)
    {
        Label* pLabel = Label::createWithTTF(mode, "fonts/Roboto-Light.ttf", 12);

        if (pLabel)
        {
            pLabel->setOpacity(0);
            pLabel->setTextColor(Color4B(0xFF, 0xFF, 0xFF, 255));
            pLabel->setPositionY(visibleSize.height * 8 / 16 + origin.y);
            pLabel->setPositionX(visibleSize.width / 2 + origin.x);

            modeLabels.pushBack(pLabel);

            this->addChild(pLabel);
        }
    }
}

void MainMenu::initBackground()
{
    GET_ORIGIN_AND_SIZE(ptOrigin, visibleSize);

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
    menuItems.pushBack(MenuItemImage::create("menu/free-play-unselected.png", "menu/free-play-selected.png", CC_CALLBACK_1(MainMenu::onPressPlay, this)));
    menuItems.pushBack(MenuItemImage::create("menu/times-unselected.png", "menu/times-selected.png", CC_CALLBACK_1(MainMenu::onPressTimes, this)));

    // Create the menu only if all menu items were created successfully
    if (std::find(menuItems.begin(), menuItems.end(), nullptr) == menuItems.end())
    {
        pMenu = Menu::createWithArray(menuItems);

        if (pMenu)
        {
            pMenu->alignItemsVerticallyWithPadding(0.0);
            pMenu->setPositionY(visibleSize.height * 5 / 16);

            this->addChild(pMenu, 3);
        }
    }
}

void MainMenu::onPressPlay(Ref* pSender)
{
    const std::string& label_string = pModeLabel->getString();

    // Find the index of the 'x' character in the string
    size_t xIndex = label_string.size() - 1;
    for (; xIndex >= 0 && label_string[xIndex] != 'x'; --xIndex) {}

    // If no such character was found, the index will be -1
    // This would only happen if some person changed the string
    // using a third party program, so in that case we just dont launh the game :)
    if (xIndex != -1)
    {
        // The reason we don't just take the next character is because the size might
        // be a 2-digit number, e.g. 12, so we take the substring from the x until the end
        std::string size = label_string.substr(xIndex + 1, label_string.size() - 1);

        // and then we convert it into a number, which we will pass to initBoard
        uint8_t boardSize = atoi(size.c_str());

        GameScene *pGameScene = GameScene::create();

        if (pGameScene)
        {
            pGameScene->initBoard(boardSize, boardSize);
            Director::getInstance()->pushScene(TransitionCrossFade::create(0.2, pGameScene));
        }
    }
}

void MainMenu::onPressTimes(Ref* pSender)
{
    BestTimesScene* pBestTimeScene = BestTimesScene::create();

    if (pBestTimeScene)
    {
        Director::getInstance()->pushScene(TransitionCrossFade::create(0.1, pBestTimeScene));
    }
}

void MainMenu::initInfoButton()
{
    ui::Button* pInfoButton = ui::Button::create("menu/information.png");

    if (pInfoButton)
    {
        GET_ORIGIN_AND_SIZE(ptOrigin, visibleSize);

        // Position this at the top right of the screen
        pInfoButton->setPosition(
                ptOrigin
                - Vec2(pInfoButton->getContentSize().width / 2, pInfoButton->getContentSize().height / 2)
                + Vec2(visibleSize.width, visibleSize.height)
        );

        addChild(pInfoButton, 5);
    }
}

void MainMenu::initModeSelector()
{
    GET_ORIGIN_AND_SIZE(ptOrigin, visibleSize);

    pScrollLeftButton = ui::Button::create("menu/backward.png");
    pScrollRightButton = ui::Button::create("menu/forward.png");

    if (pScrollLeftButton)
    {
        pScrollLeftButton->addTouchEventListener(CC_CALLBACK_2(MainMenu::onPressLeft, this));
        // Place this on the left part of the screen, centered vertically
        pScrollLeftButton->setPosition(ptOrigin + Vec2(visibleSize.width / 8, visibleSize.height / 2));

        this->addChild(pScrollLeftButton, 2);
    }

    if (pScrollRightButton)
    {
        pScrollRightButton->addTouchEventListener(CC_CALLBACK_2(MainMenu::onPressRight, this));
        // Place this on the right part of the screen, centered vertically
        pScrollRightButton->setPosition(ptOrigin + Vec2(visibleSize.width * 7 / 8, visibleSize.height * 8 / 16));

        this->addChild(pScrollRightButton, 2);
    }

    pModeLabel = modeLabels.at(iModeIndex);
    DO_IF_VALID(pModeLabel, setOpacity(0xFF));

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

static void onAdViewInit(const firebase::Future<void>& result, void* user_data)
{
    if (result.error() == firebase::gma::kAdErrorCodeNone)
    {
        auto ad_view = reinterpret_cast<firebase::gma::AdView*>(user_data);

        ad_view->SetPosition(firebase::gma::AdView::kPositionBottom);
        ad_view->Show();

        firebase::gma::AdRequest ad_request;
        firebase::Future<firebase::gma::AdResult> load_ad_request = ad_view->LoadAd(ad_request);
    }
}

static void onGmaInit(const firebase::Future<firebase::gma::AdapterInitializationStatus>& future, void* user_data)
{
    if (future.error() == firebase::gma::kAdErrorCodeNone)
    {
        std::unique_ptr<firebase::gma::AdView>& ad_view = *reinterpret_cast<std::unique_ptr<firebase::gma::AdView>*>(user_data);
        ad_view.reset(new firebase::gma::AdView());

        firebase::gma::AdParent ad_parent;
#if defined(__ANDROID__)
        ad_parent = JniHelper::getActivity();
#endif
        firebase::Future<void> result = ad_view->Initialize(ad_parent,
                                                            "ca-app-pub-3940256099942544/6300978111",
                                                            firebase::gma::AdSize::kBanner);

        result.OnCompletion(onAdViewInit, ad_view.get());
    }
}

void MainMenu::initAd()
{
    firebase::Future<firebase::gma::AdapterInitializationStatus> result = firebase::gma::InitializeLastResult();

    // Firebase initialization might have been complete by the time we get here (although unlikely)
    if (result.status() == firebase::kFutureStatusComplete)
    {
        onGmaInit(result, &ad_view);
    }

    else
    {
        result.OnCompletion(onGmaInit, &ad_view);
    }
}