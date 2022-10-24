//
// Created by Lumpros on 10/20/2022.
//

#ifndef __BEST_TIMES_SCENE_H__
#define __BEST_TIMES_SCENE_H__

#include "cocos2d.h"

class BestTimesScene : public cocos2d::Scene
{
public:
    CREATE_FUNC(BestTimesScene);

    virtual bool init() override;

private:
    void initLabels();
    void initBackground();
    void initBackButton();
};

#endif
