/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "SplashscreenScene.h"
#include "MainMenuScene.h"

#include "firebase/gma.h"
#include "firebase/gma/types.h"
#include "firebase/future.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include <platform/android/jni/JniHelper.h>
#endif

USING_NS_CC;

Scene* Splashscreen::createScene()
{
    return Splashscreen::create();
}

bool Splashscreen::init()
{
    if ( !Scene::init() )
    {
        return false;
    }

    Director* pDirector = Director::getInstance();

    const Size visibleSize = pDirector->getVisibleSize();
    const Vec2 ptOrigin = pDirector->getVisibleOrigin();

    initLanselotLabel(ptOrigin, visibleSize);
    initSoftwareLabel(ptOrigin, visibleSize);
    initFirebase();

    return true;
}

void Splashscreen::initLanselotLabel(const Vec2& ptOrigin, const Size& visibleSize)
{
    Label* pLanselotLabel = Label::createWithTTF("LANSELOT", "fonts/CloisterBlack.ttf", 20);

    if (pLanselotLabel)
    {
        pLanselotLabel->setOpacity(0);
        pLanselotLabel->setTextColor(Color4B::WHITE);
        pLanselotLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + pLanselotLabel->getContentSize().height / 2) + ptOrigin);

        this->addChild(pLanselotLabel);

        pLanselotLabel->runAction(Sequence::create(DelayTime::create(0.5), FadeIn::create(0.5), DelayTime::create(0.5), nullptr));
    }
}

void Splashscreen::initSoftwareLabel(const Vec2& ptOrigin, const Size& visibleSize)
{
    Label* pSoftwareLabel = Label::createWithTTF("SOFTWARE", "fonts/good times rg.otf", 10);

    if (pSoftwareLabel)
    {
        pSoftwareLabel->setOpacity(0);
        pSoftwareLabel->setTextColor(Color4B::WHITE);
        pSoftwareLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 5) + ptOrigin);

        this->addChild(pSoftwareLabel);

        auto fnSwapToMenu = CallFunc::create([]() {
            Director::getInstance()->replaceScene(TransitionFade::create(1.0, MainMenu::createScene()));
        });

        pSoftwareLabel->runAction(Sequence::create(DelayTime::create(0.5), FadeIn::create(0.5), DelayTime::create(0.5), fnSwapToMenu, nullptr));
    }
}

void Splashscreen::initFirebase()
{
#if defined(__ANDROID__)
    firebase::InitResult result;
    firebase::gma::Initialize(JniHelper::getEnv(), JniHelper::getActivity(), &result);
#elif
    firebase::gma::Initialize();
#endif
}