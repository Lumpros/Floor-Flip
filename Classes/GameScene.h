#ifndef __GAMESCENE_H__
#define __GAMESCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "Board.h"
#include "Timer.h"

#include "firebase/gma/interstitial_ad.h"

#include <memory>

class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    bool init() override;
    void initBoard(uint8_t rows, uint8_t columns);
    void update(float) override;
    void updateButtons();
    void doWinAftermath();

    CREATE_FUNC(GameScene);

private:
    using Button = cocos2d::ui::Button;

    std::unique_ptr<Board> pBoard = nullptr;
    std::unique_ptr<firebase::gma::InterstitialAd> interstitial_ad;

    Button* pUndoButton = nullptr;
    Button* pRedoButton = nullptr;
    Button* pRestartButton = nullptr;
    cocos2d::Label* pOnWinLabel = nullptr;
    Game::Timer timer;
    uint64_t timeSpentPlayingMilliseconds = 0;

private:
    inline void initBackgroundLayer();
    inline void initBackToMenuButton();
    inline void initControlButtons();
    inline void initInterstitialAd();
    inline void initRedoButton(const cocos2d::Vec2& ptOrigin, const cocos2d::Size& visibleSize);
    inline void initUndoButton(const cocos2d::Vec2& ptOrigin, const cocos2d::Size& visibleSize);
    inline void initRestartButton(const cocos2d::Vec2& ptOrigin, const cocos2d::Size& visibleSize);

    void doBoardWaveEffect();
    void doLabelEffect();

    void updateTimeSpentPlaying();
    void showInterstitialAd();
    bool shouldShowAd() const;

    void onRestart(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type);
    void onClose(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type);
    void onUndo(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type);
    void onRedo(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type);
};


#endif