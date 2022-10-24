//
// Created by Lumpros on 10/20/2022.
//

#include "BestTimeManager.h"
#include "cocos2d.h"

USING_NS_CC;

static const std::string g_defaultValue = "-";

static const char* g_modes[] = {
        "4x4",
        "5x5",
        "7x7",
        "9x9",
        "12x12"
};

static bool isValidMode(const std::string& mode)
{
    for (const char *pMode : g_modes)
    {
        if (mode == pMode)
        {
            return true;
        }
    }

    return false;
}

BestTimeManager* BestTimeManager::getInstance()
{
    static BestTimeManager instance;

    return &instance;
}

std::string BestTimeManager::getBestTime(const std::string& mode)
{
    return UserDefault::getInstance()->getStringForKey(mode.c_str(), g_defaultValue);
}

bool BestTimeManager::saveBestTime(const std::string& mode, const std::string& time)
{
    if (isValidMode(mode))
    {
        const std::string bestTime = getBestTime(mode);

        if (bestTime == g_defaultValue || time < bestTime)
        {
            UserDefault::getInstance()->setStringForKey(mode.c_str(), time);
            return true;
        }
    }

    return false;
}