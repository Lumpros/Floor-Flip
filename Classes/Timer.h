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
        inline std::string getString() const {
            return pLabel->getString();
        }

        void update(float delta);
        void reset(void);
        void stop(void);
        void start(void);
        void addToScene(cocos2d::Scene *);
        void setPosition(const cocos2d::Vec2&);

        inline time_t getMilliseconds() const { return milliseconds; }
        inline time_t getSeconds() const { return seconds; }
        inline time_t getMinutes() const { return minutes; }
        inline time_t getHours() const { return  hours; }

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
