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
    
    printf("Camera: target=(%d,%d) screen/zoom=%d/%d pos=(%d,%d)\n",
           targetX, targetY, screenWidth, (int)(screenWidth/(2*zoom)), x, y);
}

SDL_Rect Camera::getViewPort() {
    return viewPort;
}

void Camera::setZoom(float newZoom) {
    printf("=== SETZOOM DEBUG ===\n");
    printf("Input zoom: %.2f\n", newZoom);
    printf("screenWidth=%d, screenHeight=%d\n", screenWidth, screenHeight); // PROBLEM TUTAJ!
    
    // Jeśli te wartości są dziwne (np. 10470 zamiast 800), TO JEST BŁĄD!
    
    // Tymczasowo: ZABLOKUJ zoom na 1.0
    zoom = 1.0f;  // TYMCZASOWO!
    
    viewPort.w = (int)(screenWidth / zoom);
    viewPort.h = (int)(screenHeight / zoom);
    
    printf("Result: viewport=%dx%d\n", viewPort.w, viewPort.h);
}