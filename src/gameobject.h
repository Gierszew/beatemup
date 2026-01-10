#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "SDL2/SDL.h"
#include "camera.h"

class GameObject {
    public:
    void load(float x, float y, int width, int height, char* textureId);
    void setTexture(SDL_Renderer* renderer);
    void draw(SDL_Renderer* renderer, Camera* camera, bool flip = false);
    void update();
    void clean();

    SDL_Texture* texture;

    char* textureId;

    int originalWidth;
    int originalHeight;
    int currentFrame;
    int currentRow;
    int worldWidth;
    int worldHeight;
    float x;
    float y;
    int width;
    int height;

    float speed;
    float scale;

    bool movingLeft;
};

#endif