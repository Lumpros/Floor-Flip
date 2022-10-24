//
// Created by Lumpros on 9/30/2022.
//

#ifndef __MAINMENU_SCENE_H__
#define __MAINMENU_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "FloorTile.h"
#include "Board.h"

#include "firebase/gma/ad_view.h"

#include <memory>

class MainMenu : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    ~MainMenu();

    virtual bool init();

    CREATE_FUNC(MainMenu);

private:
    void initBackground();
    void initCocosMenu();
    void initHiddenLabels();
    void initModeSelector();
    void initPictures();
    void initInfoButton();
    void initAd();

    void onPressPlay(cocos2d::Ref* pSender);
    void onPressTimes(cocos2d::Ref* pSender);
    void onPressLeft(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);
    void onPressRight(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);

    static void shiftToCenter(cocos2d::Node* pLabel, const cocos2d::Vec2& ptOrigin, const cocos2d::Size& visibleSize);
    static void shiftToLeft(cocos2d::Node* pLabel, const cocos2d::Vec2& ptOrigin, const cocos2d::Size& visibleSize);
    static void shiftToRight(cocos2d::Node* pLabel, const cocos2d::Vec2& ptOrigin, const cocos2d::Size& visibleSize);

private:
    unsigned int iModeIndex = 0;

    cocos2d::Label* pModeLabel = nullptr;
    cocos2d::Label* pPrevLabel = nullptr;
    cocos2d::Sprite* pModeSprite = nullptr;
    cocos2d::Sprite* pPrevSprite = nullptr;
    cocos2d::Menu* pMenu = nullptr;
    cocos2d::ui::Button* pScrollLeftButton = nullptr;
    cocos2d::ui::Button* pScrollRightButton = nullptr;

    cocos2d::Vector<cocos2d::Label*> modeLabels;

    std::unique_ptr<firebase::gma::AdView> ad_view;
};


#endif
