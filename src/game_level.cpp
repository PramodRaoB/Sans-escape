#include "game_level.h"

#include<bits/stdc++.h>
using namespace std;

std::vector<std::vector<int>> GenerateLevel(int height, int width, int numCoins, int numMonsters, int numWalls);

void GameLevel::Load(unsigned int levelWidth, unsigned int levelHeight, int numCoins, int numMonsters, int numWalls)
{
    // clear old data
    walls.clear();
    monsters.clear();
    coins.clear();

    //generate level randomly given difficulty
    srand(time(NULL));
    std::vector<std::vector<int>> tileData = GenerateLevel(16, 16, numCoins, numMonsters, numWalls);
    if (!tileData.empty())
        this->init(tileData, levelWidth, levelHeight);
}


void GameLevel::Draw(SpriteRenderer &renderer)
{
    for (auto &tile: this->walls)
        tile.Draw(renderer);
    for (auto &monster: this->monsters)
        monster.Draw(renderer);
    for (auto &coin: this->coins)
        if (!coin.Destroyed)
            coin.Draw(renderer);
}

bool GameLevel::IsCompleted()
{
    return false;
}

void GameLevel::init(std::vector<std::vector<int>> tileData, unsigned int levelWidth, unsigned int levelHeight)
{
    // calculate dimensions
    unsigned int height = tileData.size();
    unsigned int width = tileData[0].size(); // note we can index vector at [0] since this function is only called if height > 0
    float unit_width = levelWidth / static_cast<float>(width), unit_height = levelHeight / height;
    // initialize level tiles based on tileData
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            // check block type from level data (2D level array)
            if (tileData[y][x] == 1)
            {
                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                GameObject obj(pos, size, ResourceManager::GetTexture("wall"));
                obj.IsSolid = true;
                this->walls.push_back(obj);
            }
            else if (tileData[y][x] == 2)	// non-solid
            {
                //coin
                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                this->coins.push_back(GameObject(pos, size, ResourceManager::GetTexture("coin")));
            }
            else if (tileData[y][x] > 2) {
                string textureName = "enemy";
                textureName += {static_cast<char>('0' + tileData[y][x] - 3)};
                cout << textureName << "\n";
                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width * 0.85f, unit_height * 0.85f);
                this->monsters.push_back(GameObject(pos, size, ResourceManager::GetTexture(textureName)));
            }
        }
    }
}

bool bfs (std::vector<std::vector<int>> &grid) {
    if (grid.empty()) return false;
    std::pair<int, int> start = {1, 1};
    std::vector<int> dx = {1, 0, 0, -1};
    std::vector<int> dy = {0, -1, 0, 1};
    int n = grid.size(), m = grid[0].size();
    std::queue<std::pair<int, int>> q;
    vector<vector<int>> visited(n, vector<int>(m, 0));
    visited[start.first][start.second] = 1;
    q.push({start.first, start.second});
    while (!q.empty()) {
        pair<int, int> curr = q.front();
        q.pop();
        for (int i = 0; i < 4; i++) {
            if (!visited[curr.first + dx[i]][curr.second + dy[i]] && grid[curr.first + dx[i]][curr.second + dy[i]] != 1) {
                visited[curr.first + dx[i]][curr.second + dy[i]] = 1;
                q.push({curr.first + dx[i], curr.second + dy[i]});
            }
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (grid[i][j] > 1 && !visited[i][j]) return false;
        }
    }
    if (!visited[n - 2][m - 2]) return false;
    return true;
}

/*
 * 0 -> empty
 * 1 -> wall
 * 2 -> coin
 * 3 - 7 -> monster
 * */
std::vector<std::vector<int>> GenerateLevel(int height, int width, int numCoins, int numMonsters, int numWalls) {
    std::vector<std::vector<int>> grid(height, std::vector<int>(width, 0));
    if (height <= 2 || width <= 2) return {};
    for (int i = 0; i < width; i++) grid[0][i] = grid[height - 1][i] = 1;
    for (int i = 0; i < height; i++) grid[i][0] = grid[i][width - 1] = 1;
    grid[1][1] = 10;
    while (numMonsters) {
        int randX = rand() % (height - 2) + 1, randY = rand() % (width - 2) + 1;
        if (randX - 1 + randY - 1 < MIN_DIST) continue;
        if (grid[randX][randY]) continue;
        grid[randX][randY] = rand() % 5 + 3;
        if (!bfs(grid)) {
            grid[randX][randY] = 0;
            continue;
        }
        numMonsters--;
    }
    while (numCoins) {
        int randX = rand() % (height - 2) + 1, randY = rand() % (width - 2) + 1;
        if (randX - 1 + randY - 1 < MIN_DIST) continue;
        if (grid[randX][randY]) continue;
        grid[randX][randY] = 2;
        if (!bfs(grid)) {
            grid[randX][randY] = 0;
            continue;
        }
        numCoins--;
    }
    while (numWalls) {
        int walkLen = rand() % MAX_WALK + 1;
        walkLen = min(walkLen, numWalls);
        int randX = rand() % (height - 2) + 1, randY = rand() % (width - 2) + 1;
        int dir = rand() % 4;
        int cnt = 0;
        pair<int, int> next = {randX, randY};
        for (int i = 0; i < walkLen; i++) {
            if (grid[next.first][next.second]) break;
            grid[next.first][next.second] = 1;
            if (!bfs(grid)) {
                grid[next.first][next.second] = 0;
                break;
            }
            if (dir == 0) next.first++;
            else if (dir == 1) next.first--;
            else if (dir == 2) next.second++;
            else next.second--;
            dir = rand() % 4;
            cnt++;
        }
        numWalls -= cnt;
    }
    grid[1][1] = 0;
    grid[width - 2][height - 1] = 0;
    return grid;
}