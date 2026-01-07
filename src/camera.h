#ifndef CAMERA_H
#define CAMERA_H

#include "SDL2/SDL.h"

class Camera {
    public:
    Camera(int width, int height);
    void update(int targetX, int targetY);
    SDL_Rect getViewPort();

    private:
    SDL_Rect viewPort;
    int width;
    int height;
    int x;
    int y;
};

#endif