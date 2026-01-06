#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "SDL2/SDL.h"

class GameObject {
    public:
    void load(int x, int y, int width, int height, char* textureId);
    void setTexture(SDL_Renderer* renderer);
    void draw(SDL_Renderer* renderer);
    void update();
    void clean();

    protected:

    SDL_Texture* texture;
    char* textureId;
    int currentFrame;
    int currentRow;
    int x;
    int y;
    int width;
    int height;
};

#endif