#include "camera.h"

Camera::Camera(int width, int height) : width(width), height(height) {
    viewPort = {0, 0, width, height};
    x = 0;
    y = 0;
}

void Camera::update(int targetX, int targetY) {
    x = targetX - width / 2;
    y = targetY - height / 2;

    // Optional: Clamp camera position to world bounds if necessary
    if (x < 0) x = 0;
    if (y < 0) y = 0;

    viewPort.x = x;
    viewPort.y = y;
}

SDL_Rect Camera::getViewPort() {
    return viewPort;
}