#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include "gameobject.h"

class Player : public GameObject {
    public:
    void load(int x, int y, int width, int height, char* textureId);
    void setTexture(SDL_Renderer* renderer);
    void draw(SDL_Renderer* renderer);
    void update();
    void clean();
};

#endif