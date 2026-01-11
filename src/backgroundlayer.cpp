#include "backgroundlayer.h"
#include <cmath>

BackgroundLayer::BackgroundLayer() {
    scrollSpeed = 0.0f;
    offsetX = 0;
    offsetY = 0;
    texture = NULL;
    width = 0;
    height = 0;
    textureId = NULL;
    floor = false;
}

BackgroundLayer::BackgroundLayer(int width, int height, bool floor) {
    scrollSpeed = 0.0f;
    offsetX = 0;
    offsetY = 0;
    texture = NULL;
    this->width = width;
    this->height = height;
    this->floor = floor;
}

void BackgroundLayer::load(SDL_Renderer* renderer, char* textureId, float scrollSpeed, bool floor) {
    this->scrollSpeed = scrollSpeed;
    this->textureId = textureId;
    this->floor = floor;
    
    printf("Loading background: %s\n", textureId);
    
    SDL_Surface* surface = SDL_LoadBMP(textureId);
    if (!surface) {
        printf("ERROR: Cannot load %s: %s\n", textureId, SDL_GetError());
        texture = NULL;
        return;
    }
    
    this->width = surface->w;
    this->height = surface->h;
    printf("Background texture size: %dx%d\n", width, height);
    
    GameObject::setTexture(renderer);
    
    SDL_FreeSurface(surface);
}

void BackgroundLayer::update(int cameraX, int cameraY) {
    if (width <= 0 || height <= 0) {
        printf("ERROR: BackgroundLayer has invalid size!\n");
        offsetX = 0;
        offsetY = 0;
        return;
    }

    offsetX = (int)(cameraX * scrollSpeed) % width;
    offsetY = (int)(cameraY * scrollSpeed) % height;
    if (offsetX < 0) offsetX += width;
    if (offsetY < 0) offsetY += height;
}

void BackgroundLayer::draw(SDL_Renderer* renderer, SDL_Rect viewport, float zoom) {
    if (!texture || width <= 0 || height <= 0) return;
    
    int worldLeft = viewport.x;
    int worldRight = viewport.x + viewport.w;
    int worldTop = viewport.y;
    int worldBottom = viewport.y + viewport.h;
    
    int startX = (int)std::floor((float)(worldLeft + offsetX) / width) - 1;
    
    int endX = (int)ceil((float)(worldRight + offsetX) / width) + 1;
    
    int startY = (int)std::floor((float)(worldTop + offsetY) / height) - 1;
    int endY = (int)ceil((float)(worldBottom + offsetY) / height) + 1;
        
    for (int x = startX; x <= endX; x++) {
        for (int y = startY; y <= endY; y++) {
            SDL_Rect dest = {
                -offsetX + (x * width) - viewport.x,
                -offsetY + (y * height) - viewport.y,
                width, height
            };
            
            if (dest.x + width > 0 && dest.x < viewport.w &&
                dest.y + height > 0 && dest.y < viewport.h) {
                SDL_RenderCopy(renderer, texture, NULL, &dest);
            }
        }
    }
}