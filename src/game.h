#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include "player.h"

class Game {
    private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* background;

    Player* player;
    Camera* camera;

    Uint32 lastTick;
    int width;
    int height;

    float deltaTime;

    bool isRunning;

    SDL_Texture* loadTexture(const char* filePath);

    public:
    bool init(const char* title, int width, int height);
    void handleEvents();
    void update();
    void render();
    void run();
    ~Game();

};

#endif