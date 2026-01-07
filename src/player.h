#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include "gameobject.h"
#include "camera.h"

class Player : public GameObject {
    public:
    void load(int x, int y, int width, int height, char* textureId);
    void setTexture(SDL_Renderer* renderer);
    void draw(SDL_Renderer* renderer, Camera* camera);
    void update(float dt);
    void clean();
    void handleInput(float dt);

};

#endif