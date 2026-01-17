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
        return true;
    }

    SDL_Surface* surface = SDL_LoadBMP(filename);
    if (!surface) {
        return false;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
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

    int px, py, c;
    SDL_Rect s, d;
    s.w = charWidth;
    s.h = charHeight;
    d.w = charWidth * scale;
    d.h = charHeight * scale;

    while (*text) {
        c = *text & 255;
        px = (c % charsPerRow) * charWidth;
        py = (c / charsPerRow) * charHeight;

        s.x = px;
        s.y = py;
        d.x = x;
        d.y = y;
        SDL_RenderCopy(renderer, texture, &s, &d);
        x += charWidth * scale;
        text++;
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