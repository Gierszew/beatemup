#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>

class Game {
    private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* background;
    int width;
    int height;
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