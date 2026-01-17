#include "gameobject.h"

GameObject::GameObject() 
    : texture(nullptr), textureId(nullptr), originalWidth(0), originalHeight(0),
      currentFrame(0), currentRow(0), worldWidth(0), worldHeight(0),
      x(0.0f), y(0.0f), width(0), height(0), speed(0.0f), scale(1.0f),
      movingLeft(false), alreadyDamaged(false) {};

void GameObject::load(float x, float y, int width, int height) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    currentFrame = 0;
    currentRow = 0;
    worldWidth = 1047;
    worldHeight = 720;
}

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
    SDL_Rect destRect = { (int)((x - viewPort.x)), (int)((y - viewPort.y)), (int)(width), (int)(height) };

    SDL_RendererFlip rFlip = flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderCopyEx(renderer, this->texture, &srcRect, &destRect, 0, NULL, rFlip);
}

void GameObject::drawWithoutTexture(SDL_Renderer* renderer, Camera* camera) {
    SDL_Rect viewPort = camera->getViewPort();
    SDL_Rect destRect = { (int)(x - viewPort.x), (int)(y - viewPort.y), (int)(width), (int)(height) };

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Szary kolor
    SDL_RenderFillRect(renderer, &destRect);
}

void GameObject::drawDebug(SDL_Renderer* renderer, Camera* camera) {
    SDL_Rect viewPort = camera->getViewPort();
    
    // Hitbox ciała (zielony kontur)
    SDL_Rect bodyRect = bodyHitbox.getRect(x, y);
    bodyRect.x -= viewPort.x;
    bodyRect.y -= viewPort.y;
    
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // Zielony
    SDL_RenderDrawRect(renderer, &bodyRect);
    
    // Wizualne obramowanie całego obiektu (niebieski)
    SDL_Rect visualRect = {
        (int)(x - viewPort.x),
        (int)(y - viewPort.y),
        width,
        height
    };
    SDL_SetRenderDrawColor(renderer, 0, 100, 255, 255);  // Niebieski
    SDL_RenderDrawRect(renderer, &visualRect);
    
    // Hitbox ataku (czerwony wypełniony)
    if (attackHitbox.width > 0 && attackHitbox.height > 0) {
        SDL_Rect attackRect = attackHitbox.getRect(x, y);
        attackRect.x -= viewPort.x;
        attackRect.y -= viewPort.y;
        
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128);  // Czerwony z przezroczystością
        SDL_RenderFillRect(renderer, &attackRect);
    }
    
    // Punkt środkowy obiektu (żółty)
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_RenderDrawPoint(renderer, 
                       (int)(x + width/2 - viewPort.x),
                       (int)(y + height/2 - viewPort.y));
}

void GameObject::update() {
    
}