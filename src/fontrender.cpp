#include "fontrender.h"
#include <stdio.h>
#include <string.h>

FontRenderer::FontRenderer() {
    texture = nullptr;
    charWidth = 8;
    charHeight = 8;
    charsPerRow = 16;
    isLoaded = false;
}

FontRenderer::~FontRenderer() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

bool FontRenderer::load(SDL_Renderer* renderer, const char* filename) {
    if (isLoaded) {
        // printf("Font already loaded.\n");
        return true;
    }

    SDL_Surface* surface = SDL_LoadBMP(filename);
    if (!surface) {
        // printf("Failed to load font bitmap: %s\n", SDL_GetError());
        return false;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        // printf("Failed to create font texture: %s\n", SDL_GetError());
        return false;
    }
    isLoaded = true;
    return true;
}

void FontRenderer::draw(SDL_Renderer* renderer, int x, int y, const char* text) {
    drawScaled(renderer, x, y, text, 1);
}

void FontRenderer::drawScaled(SDL_Renderer* renderer, int x, int y, const char* text, int scale) {
    if (!isLoaded || !texture || !text) return;

    int currentX = x;
    int scaledW = charWidth * scale;
    int scaledH = charHeight * scale;

    for (int i = 0; text[i] != '\0'; i++) {
        unsigned char c = (unsigned char)text[i];

        if (c < 32 || c > 126) {
            currentX += scaledW; // Zostaw miejsce
            continue;
        }
        
        int start_x = 0;
        int start_y = 3;
        int digit = c - '0';
        int srcX = (start_x + digit) * charWidth;
        int srcY = (start_y) * charHeight;
        
        SDL_Rect srcRect = {srcX, srcY, charWidth, charHeight};
        SDL_Rect destRect = {currentX, y, scaledW, scaledH};
        
        SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
        
        currentX += scaledW;
    }
}

int FontRenderer::textWidth(const char* text, int scale) {
    if (!text) return 0;
    
    int count = 0;
    for (int i = 0; text[i] != '\0'; i++) {
        if ((unsigned char)text[i] >= 32 && (unsigned char)text[i] <= 126) {
            count++;
        }
    }

    return count * charWidth * scale;
}