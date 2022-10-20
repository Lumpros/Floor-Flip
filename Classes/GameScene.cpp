#include "GameScene.h"
#include "MainMenuScene.h"
#include "Random.h"
#include "ui/CocosGUI.h"

#ifndef DO_IF_VALID
#define DO_IF_VALID(x, y) if (x) { (x)->y; }
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

USING_NS_CC;

const char* onWinMessages[] = {
        "Magnificent!",
        "Amazing!",
        "Great!",
        "Incredible!"
};

Scene* GameScene::createScene(void)
{
    return GameScene::create();
}

bool GameScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    const Color4B bgColor = Color4B(30, 30, 30, 255);

    LayerColor* pBackgroundLayer = LayerColor::create(bgColor);

    if (pBackgroundLayer)
    {
        this->addChild(pBackgroundLayer, -10);
    }

    initBackToMenuButton();
    initControlButtons();

    const Vec2 ptOrigin = Director::getInstance()->getVisibleOrigin();
    const Size visibleSize = Director::getInstance()->getVisibleSize();

    timer.addToScene(this);
    timer.setPosition(ptOrigin + Vec2(visibleSize.width / 2, visibleSize.height * 13 / 16));

    scheduleUpdate();

    return true;
}

void GameScene::initBackToMenuButton()
{
    Director* pDirector = Director::getInstance();

    ui::Button* pBackButton = ui::Button::create("ui/cross.png");

    if (pBackButton)
    {
        addChild(pBackButton);

        const Vec2 ptOrigin = pDirector->getVisibleOrigin();
        const Size visibleSize = pDirector->getVisibleSize();

        pBackButton->setPosition(ptOrigin + Vec2(visibleSize.width / 5, visibleSize.height * 3 / 16));

        pBackButton->addTouchEventListener(
                [=](cocos2d::Ref *sender, cocos2d::ui::Widget::TouchEventType type) {
                    if (type == ui::Widget::TouchEventType::ENDED) {
                        pBackButton->removeFromParent();
                        Director::getInstance()->popScene();
                    }
                });
    }
}

void GameScene::initControlButtons()
{
    const Vec2 ptOrigin = Director::getInstance()->getVisibleOrigin();
    const Size visibleSize = Director::getInstance()->getVisibleSize();

    pUndoButton = ui::Button::create("ui/undo-unselected.png", "ui/undo-unselected.png", "ui/undo-disabled.png");

    if (pUndoButton)
    {
        addChild(pUndoButton);

        pUndoButton->setPressedActionEnabled(true);
        pUndoButton->addTouchEventListener([=](Ref* pSender, ui::Widget::TouchEventType type) {
            if (type == ui::Widget::TouchEventType::ENDED) {
                DO_IF_VALID(pBoard, undo());
            }
        });


        pUndoButton->setPosition(ptOrigin + Vec2(visibleSize.width / 2 - pUndoButton->getBoundingBox().size.width / 2, visibleSize.height * 3 / 16));
        pUndoButton->setEnabled(false);
    }

    pRedoButton = ui::Button::create("ui/redo-unselected.png", "ui/redo-unselected.png", "ui/redo-disabled.png");

    if (pRedoButton)
    {
        addChild(pRedoButton);

        pRedoButton->setPressedActionEnabled(true);
        pRedoButton->addTouchEventListener([=](Ref* pSender, ui::Widget::TouchEventType type) {
            if (type == ui::Widget::TouchEventType::ENDED) {
                DO_IF_VALID(pBoard, redo());
            }
        });

        pRedoButton->setPosition(ptOrigin + Vec2(visibleSize.width / 2 + pRedoButton->getBoundingBox().size.width / 2, visibleSize.height * 3 / 16));
        pRedoButton->setEnabled(false);
    }

    pRestartButton = ui::Button::create("ui/restart.png");

    if (pRestartButton)
    {
        addChild(pRestartButton);

        pRestartButton->setPressedActionEnabled(true);
        pRestartButton->addTouchEventListener([&](Ref* pSender, ui::Widget::TouchEventType type) {
            if (type == ui::Widget::TouchEventType::ENDED) {
                initBoard( pBoard->getRows(), pBoard->getColumns() );
                if (pOnWinLabel)
                {
                    pOnWinLabel->stopAllActions();
                    pOnWinLabel->setOpacity(0);
                }
                timer.reset();
                timer.start();
                pUndoButton->setEnabled(false);
            }
        });

        pRestartButton->setPosition(ptOrigin + Vec2(visibleSize.width * 4 / 5, visibleSize.height * 3 / 16));
    }
}

void GameScene::initBoard(uint8_t rows, uint8_t columns)
{
    pBoard.reset(new Board(columns, rows, 10));
    pBoard->addToScene(this);
    pBoard->setPosition(Vec2(Director::getInstance()->getVisibleSize().width / 2 - pBoard->getSize() / 2, Director::getInstance()->getVisibleSize().height / 4));
    pBoard->registerEventHandlers(_eventDispatcher);
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
        const Size visibleSize = Director::getInstance()->getVisibleSize();
        const Vec2 ptOrigin = Director::getInstance()->getVisibleOrigin();

        pOnWinLabel->setString(onWinMessages[Random::randInt(ARRAY_SIZE(onWinMessages) - 1)]);
        pOnWinLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 29 / 32) + ptOrigin);

        doLabelEffect();
        doBoardWaveEffect();
    }

    timer.stop();
}

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
            const float time = 0.4f / totalRows;

            pTile->getSprite()->runAction(
                    Sequence::create(
                            DelayTime::create(0.5 + (row + col) / (float)(totalCols + totalRows)),
                            ScaleBy::create(time, 1 / scale),
                            ScaleBy::create(time, scale),
                            DelayTime::create(2 * time + (totalCols + totalRows - 2.0f - row - col)/(float)(totalRows + totalCols)),
                            MoveBy::create(time, Vec2(0, 5)),
                            MoveBy::create(time, Vec2(0, -5)),
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