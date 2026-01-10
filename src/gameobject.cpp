#include "gameobject.h"

void GameObject::load(float x, float y, int width, int height, char* textureId) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->textureId = textureId;
    currentFrame = 0;
    currentRow = 0;
    worldWidth = 1047;
    worldHeight = 720;
}

void GameObject::setTexture(SDL_Renderer* renderer) {
    if (!renderer) {
        printf("Renderer is null!\n");
        return;
    }

    printf("Loading texture: %s\n", textureId);

    SDL_Surface* surface = SDL_LoadBMP(textureId);
    if (!surface) {
        printf("Failed to load BMP: %s\n", SDL_GetError());
        return;
    }

    // Opcjonalnie wymuszenie formatu 32-bit
    SDL_Surface* formatted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(surface);
    surface = formatted;

    Uint32 key = SDL_MapRGB(surface->format, 255, 0, 255);
    SDL_SetColorKey(surface, SDL_TRUE, key);

    this->texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!this->texture) {
        printf("Failed to create texture: %s\n", SDL_GetError());
    }

    SDL_FreeSurface(surface);
}


void GameObject::draw(SDL_Renderer* renderer, Camera* camera, bool flip) {
    SDL_Rect viewPort = camera->getViewPort();
    SDL_Rect srcRect = { currentFrame * originalWidth, currentRow * originalHeight, originalWidth, originalHeight };
    printf("Drawing GameObject at position (%f, %f) with viewport offset (%d, %d)\n", x, y, viewPort.x, viewPort.y);
    SDL_Rect destRect = { (int)((x - viewPort.x)), (int)((y - viewPort.y)), (int)(width), (int)(height) };

    SDL_RendererFlip rFlip = flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderCopyEx(renderer, this->texture, &srcRect, &destRect, 0, NULL, rFlip);
}

void GameObject::update() {
    // Update logic for the game object can be implemented here
}