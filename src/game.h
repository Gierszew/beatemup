#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include "player.h"
#include "backgroundlayer.h"
#include "worldbounds.h"
#include "fontrender.h"
#include "enemy.h"

class Game {
    private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* background;

    Player* player;
    Camera* camera;
    Enemy** enemies;
    int enemyCount;
    int maxEnemies;
    BackgroundLayer backgroundLayers[3];
    int layerCount;
    WorldBounds worldBounds;
    FontRenderer font;

    Uint32 lastTick;
    Uint32 startTime;
    int width, worldWidth;
    int height, worldHeight;

    float deltaTime;

    bool isRunning;

    SDL_Texture* loadTexture(const char* filePath);

    public:
    Game();
    bool init(const char* title, int width, int height);
    void handleEvents();
    void update();
    void render();
    void run();
    void resetGame();
    void renderHUD();
    void initBackground();
    void renderBackground();

    void spawnEnemies();
    void updateEnemies(float dt);
    void renderEnemies();

    void checkCollisions();
    void resolveCollisions(Player* player, Enemy* enemy);
    ~Game();

};

#endif