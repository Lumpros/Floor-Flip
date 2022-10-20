#ifndef __GAMESCENE_H__
#define __GAMESCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "Board.h"
#include "Timer.h"

#include <memory>

class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    bool init() override;

    void update(float) override;

    void initBoard(uint8_t rows, uint8_t columns);
    void updateButtons();
    void doWinAftermath();

    CREATE_FUNC(GameScene);

private:
    std::unique_ptr<Board> pBoard = nullptr;

    cocos2d::ui::Button* pUndoButton = nullptr, *pRedoButton = nullptr;
    cocos2d::ui::Button* pRestartButton = nullptr;
    cocos2d::Label* pOnWinLabel = nullptr;

    Game::Timer timer;

private:
    inline void initBackToMenuButton();
    void initControlButtons();
    void doBoardWaveEffect();
    void doLabelEffect();
};


#endif