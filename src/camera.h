#ifndef CAMERA_H
#define CAMERA_H

#include "SDL2/SDL.h"

class Camera {
    public:
    float zoom;
    int screenWidth;
    int screenHeight;

    Camera(int width, int height);
    void update(int targetX, int targetY, int worldWidth, int worldHeight);
    SDL_Rect getViewPort();
    void setZoom(float newZoom);

    private:
    SDL_Rect viewPort;

    int x;
    int y;
};

#endif