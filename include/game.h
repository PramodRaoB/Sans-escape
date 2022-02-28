#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <bits/stdc++.h>
#include "game_level.h"

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN,
    GAME_LOSE
};

enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};
// Defines a Collision typedef that represents collision data
typedef std::tuple<bool, Direction, glm::vec2> Collision; // <collision?, what direction?, difference vector center - closest point>

// Initial size of the player
//glm::vec2 PLAYER_SIZE;
// Initial velocity of the player
const float PLAYER_VELOCITY(200.0f);
const int MONSTER_MAX_VELOCITY(90);

class Game
{
public:
    GameState               State;	
    bool                    Keys[1024];
    unsigned int            Width, Height;
    std::vector<GameLevel> Levels;
    unsigned int Level;
    int light;
    int score;
    float timePlayed;
    float lightTime;

    Game(unsigned int width, unsigned int height);
    ~Game();

    void Init();
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();

    void DoCollisions();
    void ToggleLight();

    void ResetPlayer();

    void ResetLevels();
};

#endif