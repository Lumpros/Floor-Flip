//
// Created by Lumpros on 10/2/2022.
//

#include "Board.h"
#include "Random.h"
#include "GameScene.h"

#ifndef DO_IF_VALID
#define DO_IF_VALID(x, y) if (x) { (x)->y; }
#endif

USING_NS_CC;

Board::~Board()
{
    pAnimation->release();
    cleanupTiles();
}

Board::Board(uint8_t width, uint8_t height, size_t moveBufferLength)
    : moveBufferLength { moveBufferLength }
{
    time_t t;
    srand((unsigned)time(&t));

    this->width = width;
    this->height = height;

    init();
}

void Board::init()
{
    initAnimation();
    generateSolvableBoardConfiguration();
    
    moveBufferIndex = -1;
    moves = std::vector<FloorTile*>(moveBufferLength, nullptr);
}

void Board::initAnimation()
{
    Vector<SpriteFrame*> frames = getAnimationSpriteFrames();
    pAnimation = Animation::createWithSpriteFrames(frames, 0.02);
    pAnimation->retain();
}

Vector<SpriteFrame*> Board::getAnimationSpriteFrames()
{
    SpriteFrameCache* pCache = SpriteFrameCache::getInstance();

    Vector<SpriteFrame*> frames;
    char filename[64];

    for (int i = 1; i <= 14; ++i)
    {
        sprintf(filename, "tile-%d.png", i);
        frames.pushBack(pCache->getSpriteFrameByName(filename));
    }

    return frames;
}

void Board::generateSolvableBoardConfiguration()
{
    const uint16_t rows = getRows();
    const uint16_t columns = getColumns();

    std::vector<std::vector<FloorTile::Color>> colors;

    for (size_t i = 0; i < rows; ++i)
    {
        colors.emplace_back(std::vector<FloorTile::Color>(columns));
        memset(colors[i].data(), COLOR_RED, sizeof(FloorTile::Color) * columns);
    }

    auto invertColor = [](FloorTile::Color c) {
        return !c;
    };

    // Simulate rows * columns number of moves
    for (uint16_t i = 0; i < rows * columns; ++i)
    {
        board_index randIndex = {};
        randIndex.column = Random::randInt(columns - 1);
        randIndex.row = Random::randInt(rows - 1);

        colors[randIndex.row][randIndex.column] = invertColor(colors[randIndex.row][randIndex.column]);

        for (auto index : getAdjacentIndexes(randIndex)) {
            colors[index.row][index.column] = invertColor(colors[index.row][index.column]);
        }
    }

    for (uint16_t r = 0; r < rows; ++r)
    {
        tiles.emplace_back(std::vector<FloorTile*>(columns));

        for (uint16_t c = 0; c < columns; ++c)
        {
            auto pTile = new FloorTile(colors[r][c], pAnimation);

            pTile->setBoard(this);

            tiles[r][c] = pTile;
        }
    }
}

std::vector<Board::board_index> Board::getAdjacentIndexes(Board::board_index index) const
{
    std::vector<board_index> indexes;

    auto isInBounds = [=](short row, short col) {
        return row >= 0 && row < getRows() && col >= 0 && col < getColumns();
    };

    for (short row = index.row - 1; row <= index.row + 1; ++row)
    {
        for (short col = index.column - 1; col <= index.column + 1; ++col)
        {
            if (isInBounds(row, col) && (row != index.row || col != index.column))
            {
                indexes.emplace_back(board_index(row, col));
            }
        }
    }

    return indexes;
}

void Board::cleanupTiles()
{
    std::for_each(tiles.begin(), tiles.end(), [](std::vector<FloorTile*>& row) {
        std::for_each(row.begin(), row.end(), [](FloorTile* pTile) {
            delete pTile;
        });
        row.clear();
    });

    tiles.clear();
    tiles.shrink_to_fit();
}

void Board::addToScene(cocos2d::Scene* pScene)
{
    pGameScene = (GameScene*)pScene;

    std::for_each(tiles.begin(), tiles.end(), [=](std::vector<FloorTile*>& row) {
        std::for_each(row.begin(), row.end(), [=](FloorTile* pTile) {
           pTile->addToScene(pScene);
           pTile->setScale(getTileScale());
        });
    });
}

// pos is the top left corner of the board
void Board::setPosition(const cocos2d::Vec2& pos)
{
    const int tileSize = getTileSize();

    //Since cocos2dx sets the position of the center of the sprite, we change the position by half of the tile size
    //because we're setting the position based on the top left of the board
    Vec2 ptOrigin = Director::getInstance()->getVisibleOrigin() + pos + Vec2(tileSize / 2.F, tileSize / 2.F);

    for (uint8_t row = 0; row < getRows(); ++row)
    {
        for (uint8_t col = 0; col < getColumns(); ++col)
        {
            tiles[row][col]->setPosition(ptOrigin + Vec2((float)col * tileSize, (float)row * tileSize));
        }
    }
}

float Board::getTileScale()
{
    return (float)getTileSize() / static_cast<float>(tiles[0][0]->getSprite()->getContentSize().width);
}

int Board::getTileSize() const
{
    return static_cast<int>(Director::getInstance()->getVisibleSize().width / (float)getColumns());
}

void Board::registerEventHandlers(cocos2d::EventDispatcher* _eventDispatcher)
{
    assert(_eventDispatcher);

    this->_eventDispatcher = _eventDispatcher;

    for (uint8_t row = 0; row < getRows(); ++row)
    {
        for (uint8_t col = 0; col < getColumns(); ++col)
        {
            EventListenerTouchOneByOne* pTouchListener = EventListenerTouchOneByOne::create();
            pTouchListener->onTouchBegan = CC_CALLBACK_2(FloorTile::onTouchBegan, tiles[row][col]);
            pTouchListener->onTouchEnded = CC_CALLBACK_2(FloorTile::onTouchEnded, tiles[row][col]);

            _eventDispatcher->addEventListenerWithSceneGraphPriority(pTouchListener, tiles[row][col]->getSprite());
        }
    }
}

void Board::flipAdjacentTiles(FloorTile* pFlipped)
{
    assert(pFlipped);

    std::vector<board_index> adjacentIndexes;

    for (size_t row = 0; row < getRows(); ++row)
    {
        for (size_t col = 0; col < getColumns(); ++col)
        {
            if (tiles[row][col] == pFlipped)
            {
                adjacentIndexes = getAdjacentIndexes(board_index(row, col));
                goto END_OF_OUTER_SEARCH_LOOP;
            }
        }
    }

END_OF_OUTER_SEARCH_LOOP:
    for (board_index index : adjacentIndexes)
    {
        tiles[index.row][index.column]->flip();
    }

    if (hasWon())
    {
        disableTouchHandling();

        // Simulate an empty move buffer so the buttons
        // become disabled and the player cannot alter the
        // state of the board any further.
        moveBufferIndex = -1;
        moves[0] = nullptr;
        DO_IF_VALID(pGameScene, updateButtons());
        DO_IF_VALID(pGameScene, doWinAftermath());
    }
}

int Board::getSize()
{
    return width * getTileSize();
}

bool Board::hasWon()
{
    const FloorTile::Color topLeftColor = tiles[0][0]->getColor();

    for (uint8_t row = 0; row < getRows(); ++row)
    {
        for (uint8_t col = 0; col < getColumns(); ++col)
        {
            if (tiles[row][col]->getColor() != topLeftColor)
            {
                return false;
            }
        }
    }

    return true;
}

void Board::disableTouchHandling()
{
    if (_eventDispatcher)
    {
        for (std::vector<FloorTile *> &row: tiles)
        {
            for (FloorTile *tile: row)
            {
                _eventDispatcher->removeEventListenersForTarget(tile->getSprite());
            }
        }
    }
}

void Board::shiftMovesBack()
{
    for (size_t i = 1; i < moveBufferLength; ++i)
    {
        moves[i - 1] = moves[i];
    }

    moves[moveBufferLength - 1] = nullptr;
}

void Board::undo()
{
    if (canUndoMove())
    {
        moves[moveBufferIndex]->flip();
        flipAdjacentTiles(moves[moveBufferIndex]);

        --moveBufferIndex;

        DO_IF_VALID(pGameScene, updateButtons());
    }
}

void Board::redo()
{
    if (canRedoMove())
    {
        ++moveBufferIndex;

        moves[moveBufferIndex]->flip();
        flipAdjacentTiles(moves[moveBufferIndex]);

        DO_IF_VALID(pGameScene, updateButtons());
    }
}

void Board::addTileToMoveBuffer(FloorTile* pTile)
{
    if (moveBufferIndex == moveBufferLength - 1)
    {
        shiftMovesBack();
        --moveBufferIndex;
    }

    moves[++moveBufferIndex] = pTile;

    // Once we make a move we want to delete the moves that
    // we have undone from our move history, so we null
    // the rest of the pointers.
    for (int i = moveBufferIndex + 1; i < moveBufferLength; ++i)
    {
        moves[i] = nullptr;
    }

    DO_IF_VALID(pGameScene, updateButtons());
}

bool Board::canUndoMove() const
{
    return moveBufferIndex > -1;
}

bool Board::canRedoMove() const
{
    return moveBufferIndex < static_cast<int>(moveBufferLength) - 1 && moves[moveBufferIndex + 1];
}