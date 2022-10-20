//
// Created by Lumpros on 10/11/2022.
//

#ifndef __TIMER_H__
#define __TIMER_H__

#include "cocos2d.h"

namespace Game
{
    class Timer
    {
    public:
        Timer();
        ~Timer();

        void update(float delta);
        void reset(void);
        void stop(void);
        void start(void);
        void addToScene(cocos2d::Scene *);

        void setPosition(const cocos2d::Vec2&);

    private:
        cocos2d::Label* pLabel = nullptr;

        bool isRunning = true;

        time_t milliseconds = 0;
        time_t seconds = 0;
        time_t minutes = 0;
        time_t hours = 0;

    private:
        void updateLabelString();
    };
}

#endif
