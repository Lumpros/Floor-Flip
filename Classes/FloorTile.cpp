//
// Created by Lumpros on 10/1/2022.
//

#include "FloorTile.h"
#include "Board.h"

USING_NS_CC;

FloorTile::FloorTile(FloorTile::Color color, cocos2d::Animation* pAnimation)
    : color(color)
{
    assert(pAnimation);

    if (color == COLOR_RED)
        pSprite = Sprite::createWithSpriteFrame(pAnimation->getFrames().front()->getSpriteFrame());
    else
        pSprite = Sprite::createWithSpriteFrame(pAnimation->getFrames().back()->getSpriteFrame());

    this->pAnimation = pAnimation;
}

FloorTile::~FloorTile()
{
    removeFromScene();
}

void FloorTile::setBoard(Board* pBoard)
{
    assert(pBoard);

    this->pBoard = pBoard;
}

void FloorTile::setPosition(const Vec2& pos)
{
    if (pSprite)
    {
        pSprite->setPosition(pos);
    }
}

void FloorTile::addToScene(Scene* pScene)
{
    if (pSprite)
    {
        removeFromScene();

        pScene->addChild(pSprite, 2);

        this->pScene = pScene;
    }
}

void FloorTile::removeFromScene()
{
    if (pSprite)
    {
        pSprite->removeFromParent();
    }
}

void FloorTile::flip()
{
    assert(pScene);

    if (pSprite && pAnimation)
    {
        if (color == COLOR_RED)
        {
            pSprite->runAction(Animate::create(pAnimation));
        }

        else
        {
            pSprite->runAction(Animate::create(pAnimation)->reverse());
        }

        color = (color == COLOR_RED) ? COLOR_BLUE : COLOR_RED;
    }
}

bool FloorTile::onTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{
    if (pSprite)
    {
        Vec2 ptTouch = pTouch->getLocation();
        ptTouch = pSprite->getParent()->convertToNodeSpace(ptTouch);

        return pSprite->getBoundingBox().containsPoint(ptTouch);
    }

    return false;
}

bool FloorTile::onTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{
    assert(pBoard);

    if (pSprite)
    {
        Vec2 ptTouch = pTouch->getLocation();
        ptTouch = pSprite->getParent()->convertToNodeSpace(ptTouch);

        if (pSprite->getBoundingBox().containsPoint(ptTouch))
        {
            pBoard->addTileToMoveBuffer(this);
            flip();
            pBoard->flipAdjacentTiles(this);
            return true;
        }
    }

    return false;
}

void FloorTile::setScale(float scale)
{
    if (pSprite)
    {
        pSprite->setScale(scale);
    }
}