#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "SDL2/SDL.h"

class GameObject {
    public:
    void load(float x, float y, int width, int height, char* textureId);
    void setTexture(SDL_Renderer* renderer);
    void draw(SDL_Renderer* renderer, SDL_Rect viewPort);
    void update();
    void clean();

    SDL_Texture* texture;

    char* textureId;

    int currentFrame;       // pozniej do wywalenia
    int currentRow;         // pozniej do wywalenia
    float x;
    float y;
    int width;
    int height;

    float speed;
};

#endif