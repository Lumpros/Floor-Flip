//
// Created by Lumpros on 10/11/2022.
//

#include "Timer.h"

USING_NS_CC;

// Delta is in seconds
void Game::Timer::update(float delta)
{
    if (!isRunning)
    {
        return;
    }

    milliseconds += static_cast<time_t>(delta * 1000.F);

    if (milliseconds >= 1000L)
    {
        milliseconds -= 1000L;
        seconds += 1L;

        if (seconds >= 60L)
        {
            seconds = 0L;
            minutes += 1L;

            if (minutes >= 60L)
            {
                minutes = 0L;
                hours += 1L;

                if (hours >= 100L)
                {
                    hours = 0L;
                }
            }
        }
    }

    updateLabelString();
}

void Game::Timer::reset()
{
    hours = minutes = seconds = milliseconds = 0;
}

void Game::Timer::stop()
{
    isRunning = false;
}

void Game::Timer::start()
{
    isRunning = true;
}

void Game::Timer::addToScene(Scene* pScene)
{
    assert(pScene);

    pLabel = Label::createWithTTF("00:00:00.000", "fonts/Roboto-Light.ttf", 20);

    if (pLabel)
    {
        pScene->addChild(pLabel);
    }
}

void Game::Timer::updateLabelString()
{
    if (pLabel)
    {
        char buffer[64];
        sprintf(buffer, "%02ld:%02ld:%02ld.%03ld", hours, minutes, seconds, milliseconds);

        pLabel->setString(buffer);
    }
}

void Game::Timer::setPosition(const cocos2d::Vec2& pos)
{
    if (pLabel)
    {
        pLabel->setPosition(pos);
    }
}