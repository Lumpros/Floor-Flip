#include "GameScene.h"
#include "MainMenuScene.h"
#include "Random.h"
#include "BestTimeManager.h"
#include "ui/CocosGUI.h"

#include "firebase/gma/interstitial_ad.h"
#include "firebase/gma/types.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include <platform/android/jni/JniHelper.h>
#endif

#ifndef DO_IF_VALID
#define DO_IF_VALID(x, y) if (x) { (x)->y; }
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef GET_ORIGIN_AND_SIZE
#define GET_ORIGIN_AND_SIZE(origin_name, size_name) \
        Director* pDirector = Director::getInstance(); \
        const Vec2 origin_name = pDirector->getVisibleOrigin(); \
        const Size size_name = pDirector->getVisibleSize();
#endif

USING_NS_CC;

const char* onWinMessages[] = {
        "Magnificent!",
        "Amazing!",
        "Great!",
        "Incredible!"
};

const char* ad = "ca-app-pub-3940256099942544/1033173712";

Scene* GameScene::createScene()
{
    return GameScene::create();
}

bool GameScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    initBackgroundLayer();
    initBackToMenuButton();
    initControlButtons();
    initInterstitialAd();

    timer.addToScene(this);

    scheduleUpdate();

    return true;
}

void GameScene::initBackgroundLayer()
{
    LayerColor* pBackgroundLayer = LayerColor::create(Color4B(30, 30, 30, 255));

    if (pBackgroundLayer)
    {
        this->addChild(pBackgroundLayer, -10);
    }
}

void GameScene::initBackToMenuButton()
{
    ui::Button* pBackButton = ui::Button::create("ui/cross.png");

    if (pBackButton)
    {
        GET_ORIGIN_AND_SIZE(ptOrigin, visibleSize);

        // Position it right under the board, to the left of the screen
        pBackButton->setPosition(ptOrigin + Vec2(visibleSize.width / 5, visibleSize.height * 3 / 16));
        pBackButton->addTouchEventListener(CC_CALLBACK_2(GameScene::onClose, this));

        this->addChild(pBackButton);
    }
}

void GameScene::onClose(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type)
{
    if (type == ui::Widget::TouchEventType::ENDED)
    {
        updateTimeSpentPlaying();

        if (shouldShowAd())
        {
            showInterstitialAd();
            timeSpentPlayingMilliseconds = 0;
        }

        Director::getInstance()->popScene();
    }
}

void GameScene::initControlButtons()
{
    GET_ORIGIN_AND_SIZE(ptOrigin, visibleSize);

    initUndoButton(ptOrigin, visibleSize);
    initRedoButton(ptOrigin, visibleSize);
    initRestartButton(ptOrigin, visibleSize);
}

void GameScene::initUndoButton(const cocos2d::Vec2& ptOrigin, const cocos2d::Size& visibleSize)
{
    pUndoButton = ui::Button::create("ui/undo-unselected.png", "ui/undo-unselected.png", "ui/undo-disabled.png");

    if (pUndoButton)
    {
        pUndoButton->setEnabled(false);
        pUndoButton->setPressedActionEnabled(true);
        pUndoButton->addTouchEventListener(CC_CALLBACK_2(GameScene::onUndo, this));

        // Position it right under the board, slightly to the left of the center of the screen
        pUndoButton->setPosition(ptOrigin + Vec2(visibleSize.width / 2 - pUndoButton->getBoundingBox().size.width / 2,
                                                 visibleSize.height * 3 / 16));

        this->addChild(pUndoButton);
    }
}

void GameScene::onUndo(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type)
{
    if (type == ui::Widget::TouchEventType::ENDED)
    {
        DO_IF_VALID(pBoard, undo());
    }
}

void GameScene::initRedoButton(const cocos2d::Vec2& ptOrigin, const cocos2d::Size& visibleSize)
{
    pRedoButton = ui::Button::create("ui/redo-unselected.png", "ui/redo-unselected.png", "ui/redo-disabled.png");

    if (pRedoButton)
    {
        pRedoButton->setPressedActionEnabled(true);
        pRedoButton->setEnabled(false);
        pRedoButton->addTouchEventListener(CC_CALLBACK_2(GameScene::onRedo, this));

        // Position it right under the board, slightly to the right of the center of the screen
        pRedoButton->setPosition(ptOrigin + Vec2(visibleSize.width / 2 +
                                                 pRedoButton->getBoundingBox().size.width / 2,
                                                 visibleSize.height * 3 / 16));

        this->addChild(pRedoButton);
    }
}

void GameScene::onRedo(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type)
{
    if (type == ui::Widget::TouchEventType::ENDED)
    {
        DO_IF_VALID(pBoard, redo());
    }
}

void GameScene::initRestartButton(const cocos2d::Vec2& ptOrigin, const cocos2d::Size& visibleSize)
{
    pRestartButton = ui::Button::create("ui/restart.png");

    if (pRestartButton)
    {
        pRestartButton->setPressedActionEnabled(true);
        pRestartButton->addTouchEventListener(CC_CALLBACK_2(GameScene::onRestart, this));

        // Position it right under the board, to the right of the screen
        pRestartButton->setPosition(ptOrigin + Vec2(visibleSize.width * 4 / 5, visibleSize.height * 3 / 16));

        this->addChild(pRestartButton);
    }
}

void GameScene::onRestart(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type)
{
    if (type == ui::Widget::TouchEventType::ENDED)
    {
        updateTimeSpentPlaying();

        if (shouldShowAd())
        {
            showInterstitialAd();
            timeSpentPlayingMilliseconds = 0;
        }

        initBoard( pBoard->getRows(), pBoard->getColumns() );

        if (pOnWinLabel)
        {
            pOnWinLabel->stopAllActions();
            pOnWinLabel->setOpacity(0);
        }

        timer.reset();
        timer.start();

        DO_IF_VALID(pUndoButton, setEnabled(false));
    }
}

void GameScene::showInterstitialAd()
{
    // Realistically speaking, the future will have finished by the time we get to this point
    // because we will call this after a long time of playing, however, we will check
    // that it has actually finished in order to be more conventional
    if (interstitial_ad->LoadAdLastResult().status() == firebase::kFutureStatusComplete)
    {
        if (interstitial_ad->LoadAdLastResult().error() == firebase::gma::kAdErrorCodeNone)
        {
            interstitial_ad->Show();

            // Once the player has closed the ad, we can go ahead and request one immediately
            // so that the next time we want to show an ad, there may be one available already
            interstitial_ad->LoadAd(ad, firebase::gma::AdRequest());
        }
    }
}

void GameScene::initBoard(uint8_t rows, uint8_t columns)
{
    GET_ORIGIN_AND_SIZE(ptOrigin, visibleSize);

    pBoard.reset(new Board(columns, rows, 10));

    if (pBoard)
    {
        // Position this in the middle of the screen. This is relative to the bottom left corner of the board
        pBoard->setPosition(Vec2(visibleSize.width / 2 - pBoard->getSize() / 2, visibleSize.height / 4));
        pBoard->registerEventHandlers(_eventDispatcher);
        pBoard->addToScene(this);

        // And position this slightly above the board, centered horizontally
        timer.setPosition(ptOrigin + Vec2(visibleSize.width / 2, pBoard->getPosition().y + visibleSize.width / 8));
    }
}

static void onInitInterstitialAd(const firebase::Future<void>& future, void* user_data)
{
    if (future.error() == firebase::gma::kAdErrorCodeNone)
    {
        auto interstitial_ad = reinterpret_cast<firebase::gma::InterstitialAd*>(user_data);

        firebase::gma::AdRequest ad_request;
        interstitial_ad->LoadAd(ad, ad_request);
    }
}

void GameScene::initInterstitialAd()
{
   interstitial_ad.reset(new firebase::gma::InterstitialAd());

   if (interstitial_ad)
   {
       firebase::gma::AdParent ad_parent;
#if defined(__ANDROID__)
       ad_parent = JniHelper::getActivity();
#endif
       interstitial_ad->Initialize(ad_parent).OnCompletion(onInitInterstitialAd, interstitial_ad.get());
   }
}

void GameScene::updateButtons()
{
    if (pBoard->canUndoMove()) {
        DO_IF_VALID(pUndoButton, setEnabled(true));
    } else {
        DO_IF_VALID(pUndoButton, setEnabled(false));
    }

    if (pBoard->canRedoMove()) {
        DO_IF_VALID(pRedoButton, setEnabled(true));
    } else {
        DO_IF_VALID(pRedoButton, setEnabled(false));
    }
}

void GameScene::doWinAftermath()
{
    if (!pOnWinLabel)
    {
        pOnWinLabel = Label::createWithTTF("", "fonts/Roboto-Light.ttf", 20);

        if (pOnWinLabel)
        {
            addChild(pOnWinLabel);
        }
    }

    if (pOnWinLabel)
    {
        GET_ORIGIN_AND_SIZE(ptOrigin, visibleSize)

        pOnWinLabel->setString(onWinMessages[Random::randInt(ARRAY_SIZE(onWinMessages) - 1)]);

        // Position this in the center of the screen (horizontally), slightly above the timer
        pOnWinLabel->setPosition(Vec2(visibleSize.width / 2, pBoard->getPosition().y + visibleSize.width * 2 / 7) + ptOrigin);

        doLabelEffect();
        doBoardWaveEffect();
    }

    timer.stop();

    // The best time manager uses the size of the board as a key (e.g. "4x4", "7x7")
    // So we construct a string using the dimensions of the board
    std::string mode = std::to_string(pBoard->getRows()) + "x" + std::to_string(pBoard->getColumns());

    // and then we pass it to the manager
    BestTimeManager::getInstance()->saveBestTime(mode, timer.getString());
}

// The effect is basically making the label bigger with a bouncing effect, whilst fading it in
void GameScene::doLabelEffect()
{
    const uint8_t totalRows = pBoard->getRows();
    const uint8_t totalCols = pBoard->getColumns();

    pOnWinLabel->setOpacity(0);
    pOnWinLabel->setScale(0.5);

    // The delayed time is basically the time it will take for the last tile to start scaling in and out
    // Since we use the formula (0.5 + (row + col) / (totalRows + totalCols)), we can substitute
    // row = totalRows - 1 and col = totalCols - 1, because these are the indexes of the
    // last tile, so we get the result below
    auto pDelayTime = DelayTime::create(0.5F + (totalCols + totalRows - 2.F) / (float)(totalCols + totalRows) + 0.8 / totalRows);
    auto pSpawn = Spawn::create(FadeIn::create(0.2F), EaseBackInOut::create(ScaleTo::create(0.4, 1.0)), nullptr);

    Sequence* pSequence = Sequence::create(pDelayTime, pSpawn, nullptr);

    pOnWinLabel->runAction(pSequence);
}

// The 'wave' effect is basically scaling all the tiles in a diagonal way,
// starting from the bottom left tile and ending on the top right tile
void GameScene::doBoardWaveEffect()
{
    const uint8_t totalRows = pBoard->getRows();
    const uint8_t totalCols = pBoard->getColumns();

    for (uint8_t row = 0; row < totalRows; ++row)
    {
        for (uint8_t col = 0; col < totalCols; ++col)
        {
            FloorTile* pTile = pBoard->getTile(row, col);

            constexpr float scale = 1 / 1.2f;
            const float timeToScale = 0.4f / totalRows;

            pTile->runAction(
                    Sequence::create(
                            // Wait until the tile should be scaled, depends on the position on the board
                            DelayTime::create(0.5F + (float)(row + col) / (float)(totalCols + totalRows)),
                            // Scale it up
                            ScaleBy::create(timeToScale, 1 / scale),
                            // Scale it down
                            ScaleBy::create(timeToScale, scale),
                            // Wait until all the tiles have finished scaling up and down
                            DelayTime::create(2 * timeToScale + (totalCols + totalRows - 2.0f - row - col)/(float)(totalRows + totalCols)),
                            // Move up
                            MoveBy::create(timeToScale, Vec2(0, 5)),
                            // Move down
                            MoveBy::create(timeToScale, Vec2(0, -5)),
                            nullptr
                    )
            );
        }
    }
}

void GameScene::update(float delta)
{
    timer.update(delta);
}

bool GameScene::shouldShowAd() const
{
    // 5 minutes
    return timeSpentPlayingMilliseconds >= 1000 * 60 * 5;
}

void GameScene::updateTimeSpentPlaying()
{
    // We add the milliseconds stored in the timer to the total time, since we assume that the timer
    // will be reset after calling this function
    timeSpentPlayingMilliseconds += timer.getMilliseconds() + 1000 * (timer.getSeconds() + 60 * (timer.getMinutes() + 60 * timer.getHours()));
}