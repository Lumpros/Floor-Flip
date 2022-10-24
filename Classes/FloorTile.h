//
// Created by Lumpros on 10/1/2022.
//

#ifndef __FLOOR_TILE_H__
#define __FLOOR_TILE_H__

#include "cocos2d.h"

#define COLOR_RED 0
#define COLOR_BLUE 1

class Board;

class FloorTile
{
public:
    typedef unsigned char Color;

    FloorTile(Color, cocos2d::Animation*);
    ~FloorTile();

    inline Color getColor() const { return color; }
    inline cocos2d::Sprite* getSprite() const { return pSprite; }

    bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*);
    bool onTouchEnded(cocos2d::Touch*, cocos2d::Event*);
    void setPosition(const cocos2d::Vec2&);
    void setScale(float scale);
    void setBoard(Board* pBoard);
    void addToScene(cocos2d::Scene*);
    void removeFromScene();
    void flip();

    void runAction(cocos2d::Action* action);

private:
    cocos2d::Scene* pScene = nullptr;
    cocos2d::Sprite* pSprite = nullptr;
    cocos2d::Animation* pAnimation = nullptr;

    Color color;
    Board* pBoard = nullptr;
};


#endif
