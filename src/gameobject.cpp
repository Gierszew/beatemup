#include "gameobject.h"

void GameObject::load(int x, int y, int width, int height, char* textureId) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->textureId = textureId;
    currentFrame = 1;
    currentRow = 1;
}

void GameObject::setTexture(SDL_Renderer* renderer) {
    SDL_Surface* surface = SDL_LoadBMP(textureId);
    this->texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

void GameObject::draw(SDL_Renderer* renderer) {
    SDL_Rect srcRect = { currentFrame * width, currentRow * height, width, height };
    SDL_Rect destRect = { x, y, width, height };
    SDL_RenderCopy(renderer, this->texture, &srcRect, &destRect);
}

void GameObject::update() {
    // Update logic for the game object can be implemented here
}