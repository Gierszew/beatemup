#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include "player.h"
#include "backgroundlayer.h"
#include "worldbounds.h"
#include "fontrender.h"
#include "enemy.h"
#include "levelloader.h"
#include "menu.h"

enum GameState {
    STATE_MENU,
    STATE_PLAYING,
    STATE_GAMEOVER
};

class Game {
    private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* background;

    Menu* menu;
    GameState currentState;

    Uint32 lastTick;
    Uint32 startTime;
    Uint32 playStartTime;

    int width, worldWidth;
    int height, worldHeight;
    int currentLevel;
    const int MAX_LEVELS = 2;

    float deltaTime;

    bool isRunning;

    SDL_Texture* loadTexture(const char* filePath);

    public:
    bool debugMode;

    Player* player;
    Camera* camera;

    Enemy** enemies;
    int enemyCount;
    int maxEnemies;

    BackgroundLayer backgroundLayers[3];
    int layerCount;
    WorldBounds worldBounds;
    FontRenderer font;

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
    void loadLevel(int levelNum);
    void renderDebugInfo();

    void spawnEnemies();
    void updateEnemies(float dt);
    void renderEnemies();

    void checkCollisions();
    void resolveCollisions(GameObject* obj1, GameObject* obj2);
    
    void drawComboUI();

    SDL_Renderer* getRenderer() { return renderer; }
    ~Game();

};

#endif