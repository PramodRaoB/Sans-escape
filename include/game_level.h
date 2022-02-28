#ifndef GAMELEVEL_H
#define GAMELEVEL_H
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "sprite_renderer.h"
#include "resource_manager.h"


/// GameLevel holds all Tiles as part of a Breakout level and 
/// hosts functionality to Load/render levels from the harddisk.
#define MIN_DIST 5
#define MAX_WALK 10
class GameLevel
{
public:
    // level state
    std::vector<GameObject> walls;
    std::vector<GameObject> monsters;
    std::vector<GameObject> coins;
    // constructor
    GameLevel() = default;
    // loads level from file
    void Load(unsigned int levelWidth, unsigned int levelHeight, int numCoins, int numMonsters, int numWalls);
    // render level
    void Draw(SpriteRenderer &renderer);
    // check if the level is completed (all non-solid tiles are destroyed)
    bool IsCompleted();
private:
    // initialize level from tile data
    void init(std::vector<std::vector<int>> tileData, unsigned int levelWidth, unsigned int levelHeight);
};

#endif