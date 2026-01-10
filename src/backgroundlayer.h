#ifndef BACKGROUNDLAYER_H
#define BACKGROUNDLAYER_H

#include <SDL2/SDL.h>
#include "gameobject.h"

class BackgroundLayer : public GameObject {
    private:
    float scrollSpeed;
    int offsetX, offsetY;
    bool floor;

    public:
    BackgroundLayer();
    BackgroundLayer(int width, int height, bool floor = false);
    void load(SDL_Renderer* renderer, char* textureId, float scrollSpeed, bool floor = false);
    void update(int cameraX, int cameraY);
    void draw(SDL_Renderer* renderer, SDL_Rect viewport, float zoom);
};

#endif