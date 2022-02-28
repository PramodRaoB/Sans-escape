#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "game_level.h"
#include "text_renderer.h"

// Game-related State data
SpriteRenderer *Renderer;
GameObject *Player;
TextRenderer *Text;

Game::Game(unsigned int width, unsigned int height)
        : State(GAME_MENU), Keys(), Width(width), Height(height), light(1), score(0), timePlayed(0), lightTime(0) {

}

Game::~Game() {
    delete Renderer;
    delete Player;
    delete Text;
}

const std::vector<int> speeds = {80, 100, 130};
bool CheckCollision(GameObject &one, GameObject &two);
void ResolveCollision(GameObject &one, GameObject &two);

void Game::Init() {
    //load shaders
    ResourceManager::LoadShader("../src/sprite.vs", "../src/sprite.fs", nullptr, "sprite");

    //configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("sprite").Use().SetMatrix4("projection", projection);

    //load textures
    ResourceManager::LoadTexture("../resources/background_test.png", true, "background");
    ResourceManager::LoadTexture("../resources/wall_test.jpg", false, "wall");
    ResourceManager::LoadTexture("../resources/player_test.png", true, "player");
    ResourceManager::LoadTexture("../resources/enemy0.png", true, "enemy0");
    ResourceManager::LoadTexture("../resources/enemy1.png", true, "enemy1");
    ResourceManager::LoadTexture("../resources/enemy2.png", true, "enemy2");
    ResourceManager::LoadTexture("../resources/enemy3.png", true, "enemy3");
    ResourceManager::LoadTexture("../resources/enemy4.png", true, "enemy4");
    ResourceManager::LoadTexture("../resources/coin.png", true, "coin");

    //Render stuff
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    ResourceManager::GetShader("sprite").Use().SetInteger("light", 1);
    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("../resources/bison.ttf", 24);

    //load levels
    GameLevel one; one.Load(this->Width, this->Height, 5, 3, 25);
    GameLevel two; two.Load(this->Width, this->Height, 6, 5, 30);
    GameLevel three; three.Load(this->Width, this->Height, 7, 7, 40);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);

    //Game object stuff
    float unit_width = this->Width / static_cast<float>(16), unit_height = this->Height / static_cast<float>(16);
    glm::vec2 playerPos = glm::vec2(unit_width * 1, unit_height * 1);
    Player = new GameObject(playerPos, glm::vec2 (unit_width * 0.85f, unit_height * 0.85f), ResourceManager::GetTexture("player"));
}

void Game::Update(float dt) {
    //move objects
    if (this->State == GAME_ACTIVE) {
        this->timePlayed += dt;
        if (!this->light) this->lightTime += dt;
        ResourceManager::GetShader("sprite").Use().SetVector4f("lightPos", glm::vec4(Player->Position.x + Player->Size.x / 2, Player->Position.y + Player->Size.y / 2, 0.0f, 1.0f));
        for (auto &enemy: this->Levels[this->Level].monsters) {
            int speed = speeds[this->Level];
            float velocity = speed * dt;
            float newDx = 0.0f, newDy = 0.0f;
            int changes = 0;
            if (enemy.Position.x > Player->Position.x) {
                enemy.Position.x -= velocity;
                newDx -= velocity;
                changes = 1;
            }
            if (enemy.Position.x < Player->Position.x) {
                enemy.Position.x += velocity;
                newDx += velocity;
                changes = 1;
            }
            enemy.deltaX = newDx;
            for (auto &wall: this->Levels[this->Level].walls) {
                if (CheckCollision(enemy, wall)) {
                    enemy.Position.x -= newDx;
                }
            }
            if (enemy.Position.y > Player->Position.y) {
                enemy.Position.y -= velocity;
                newDy -= velocity;
                changes = 1;
            }
            if (enemy.Position.y < Player->Position.y) {
                enemy.Position.y += velocity;
                newDy += velocity;
                changes = 1;
            }
            if (changes) {
                enemy.deltaY = newDy, enemy.deltaX = newDx;
            }
        }

        //iscompleted?
        if (Player->Position.x > this->Width) {
            if (this->Level < 2) {
                this->Level++;
                this->ResetPlayer();
            }
            else this->State = GAME_WIN;
        }
        //collissions
        this->DoCollisions();
    }
}

void Game::ProcessInput(float dt) {
    if (this->State == GAME_ACTIVE) {
        float velocity = PLAYER_VELOCITY * dt;
        // move player
        float newDx = 0.0f, newDy = 0.0f;
        int changes = 0;
        if (this->Keys[GLFW_KEY_A]) {
            if (Player->Position.x >= 0.0f) {
                Player->Position.x -= velocity;
                newDx -= velocity;
                changes = 1;
            }
        }
        if (this->Keys[GLFW_KEY_D]) {
                Player->Position.x += velocity;
                newDx += velocity;
                changes = 1;
        }
        if (this->Keys[GLFW_KEY_W]) {
                Player->Position.y -= velocity;
                newDy -= velocity;
                changes = 1;
        }
        if (this->Keys[GLFW_KEY_S]) {
                Player->Position.y += velocity;
                newDy += velocity;
                changes = 1;
        }
        if (changes) {
            Player->deltaX = newDx, Player->deltaY = newDy;
        }
    }
}

void Game::ResetLevels() {
    this->Levels[0].Load(this->Width, this->Height, 5, 3, 25);
    this->Levels[1].Load(this->Width, this->Height, 6, 5, 30);
    this->Levels[2].Load(this->Width, this->Height, 7, 7, 40);
    this->score = 0;
    this->Level = 0;
    this->timePlayed = 0;
    this->lightTime = 0;
}

void Game::ResetPlayer() {
    float unit_width = this->Width / static_cast<float>(16), unit_height = this->Height / static_cast<float>(16);
    Player->Position.x = unit_width;
    Player->Position.y = unit_height;
    Player->deltaY = -5.0f, Player->deltaX = -5.0f;
}

void Game::Render() {
    if (this->State == GAME_ACTIVE) {
        //draw background
        Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
        //draw level
        this->Levels[Level].Draw(*Renderer);
        //draw player
        Player->Draw(*Renderer);
        std::stringstream ss; ss << this->score;
        Text->RenderText("Score: " + ss.str(), 5.0f, 5.0f, 1.0f);
        std::stringstream iss; iss << this->timePlayed;
        Text->RenderText("Time Played: " + iss.str(), 100.0f, 5.0f, 1.0f);
        std::stringstream isss; isss << this->lightTime;
        Text->RenderText("Time Played in hardcore mode: " + isss.str(), 300.0f, 5.0f, 1.0f);
    }
    if (this->State == GAME_MENU)
    {
        Text->RenderText("SANS ESCAPE!", this->Width / 2.0 - 150.0f, this->Height / 2.0f - 50.0, 2.0f);

        Text->RenderText("Press ENTER to start", 250.0f, this->Height / 2.0f, 1.0f);
        Text->RenderText("W,S,A,D to move around and Spacebar to turn off the lights", 245.0f, this->Height / 2.0f + 20.0f, 0.75f);
    }
    if (this->State == GAME_WIN)
    {
        Text->RenderText("You WON!!!", 320.0f, this->Height / 2.0f - 20.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        std::stringstream ss; ss << this->timePlayed;
        Text->RenderText("Time Played: " + ss.str(), 5.0f, 100.0f, 1.0f);
        std::stringstream isss; isss << this->lightTime;
        Text->RenderText("Time Played in hardcore mode!: " + isss.str(), 200.0f, 100.0f, 1.0f);
        Text->RenderText("Press ENTER to retry or ESC to quit", 130.0f, this->Height / 2.0f, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
    }
    if (this->State == GAME_LOSE) {
        Text->RenderText("You LOST!!!", 320.0f, this->Height / 2.0f - 20.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        std::stringstream ss; ss << this->timePlayed;
        Text->RenderText("Time Played: " + ss.str(), 5.0f, 100.0f, 1.0f);
        std::stringstream isss; isss << this->lightTime;
        Text->RenderText("Time Played in hardcore mode!: " + isss.str(), 200.0f, 100.0f, 1.0f);
        Text->RenderText("Press ENTER to retry or ESC to quit", 130.0f, this->Height / 2.0f, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
    }
}


void Game::DoCollisions() {
    for (auto &wall: this->Levels[this->Level].walls) {
        if (CheckCollision(*Player, wall)) {
            ResolveCollision(*Player, wall);
        }
    }
    for (auto &wall: this->Levels[this->Level].walls) {
        for (auto &enemy: this->Levels[this->Level].monsters) {
            if (CheckCollision(enemy, wall)) {
                ResolveCollision(enemy, wall);
            }
        }
    }

    for (auto &enemy: this->Levels[this->Level].monsters) {
        if (CheckCollision(*Player, enemy)) {
            this->State = GAME_LOSE;
        }
    }

    for (auto &coin: this->Levels[this->Level].coins) {
        if (CheckCollision(*Player, coin) && !coin.Destroyed) {
            coin.Destroyed = true;
            this->score += 10;
        }
    }
}

void Game::ToggleLight() {
    this->light ^= 1;
    ResourceManager::GetShader("sprite").Use().SetInteger("light", this->light);
}

void ResolveCollision(GameObject &one, GameObject &two) {
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
                      two.Position.x + two.Size.x >= one.Position.x;
    if (collisionX) one.Position.x -= one.deltaX;
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
                      two.Position.y + two.Size.y >= one.Position.y;
    if (collisionY) one.Position.y -= one.deltaY;
}

bool CheckCollision(GameObject &one, GameObject &two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
                      two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
                      two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}