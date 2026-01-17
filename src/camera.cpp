#include "camera.h"

Camera::Camera(int width, int height) {
    viewPort = {0, 0, width, height};
    screenWidth = 1047;
    screenHeight = 720;
    x = 0;
    y = 0;
    zoom = 1.0f;
}

void Camera::update(int targetX, int targetY, int worldWidth, int worldHeight) {
    x = targetX - (int)(screenWidth / (2 * zoom));
    y = (screenHeight / 2) - (viewPort.h / 2);
    
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x > worldWidth - viewPort.w) x = worldWidth - viewPort.w;
    if (y > worldHeight - viewPort.h) y = worldHeight - viewPort.h;
    
    viewPort.x = x;
    viewPort.y = y;
    
}

SDL_Rect Camera::getViewPort() {
    return viewPort;
}

void Camera::setZoom(float newZoom) {
    zoom = 1.0f;
    
    viewPort.w = (int)(screenWidth / zoom);
    viewPort.h = (int)(screenHeight / zoom);
    
}