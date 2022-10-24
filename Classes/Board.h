//
// Created by Lumpros on 10/2/2022.
//

#ifndef __BOARD_H__
#define __BOARD_H__

#include "cocos2d.h"

#include "FloorTile.h"

#include <stdint.h>
#include <vector>

class GameScene;

class Board
{
public:
    void addToScene(cocos2d::Scene* pScene);
    void setPosition(const cocos2d::Vec2& pos);
    cocos2d::Vec2 getPosition(void) const;
    void registerEventHandlers(cocos2d::EventDispatcher*);
    void flipAdjacentTiles(FloorTile* pFlipped);
    void addTileToMoveBuffer(FloorTile* pTile);
    void undo();
    void redo();
    bool hasWon();

    bool canUndoMove() const;
    bool canRedoMove() const;

    inline uint8_t getColumns() const {  return width; }
    inline uint8_t getRows() const { return height; }

    int getSize();

    inline FloorTile* getTile(uint8_t row, uint8_t col) {
        return tiles[row][col];
    }

    Board(uint8_t width, uint8_t height, size_t moveBufferLength);
    ~Board();

private:
    std::vector<std::vector<FloorTile*>> tiles;

    std::vector<FloorTile*> moves;
    size_t moveBufferLength;
    signed int moveBufferIndex;

    cocos2d::Animation* pAnimation = nullptr;
    cocos2d::EventDispatcher* _eventDispatcher = nullptr;
    GameScene* pGameScene = nullptr;

    uint8_t width, height;

    struct board_index {
      uint8_t row;
      uint8_t column;
      board_index() = default;
      board_index(uint8_t row, uint8_t column)
       : row(row), column(column) {}
    };

private:
    static cocos2d::Vector<cocos2d::SpriteFrame*> getAnimationSpriteFrames();
    std::vector<board_index> getAdjacentIndexes(board_index index) const;

    void init();
    void initAnimation();
    void generateSolvableBoardConfiguration();
    void cleanupTiles();

    void disableTouchHandling();
    void shiftMovesBack();

    float getTileScale();
    int getTileSize() const;
};


#endif
