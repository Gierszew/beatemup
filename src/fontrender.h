#ifndef FONTRENDERER_H
#define FONTRENDERER_H

#include <SDL2/SDL.h>

class FontRenderer {
    private:
    SDL_Texture* texture;

    int charWidth, charHeight;
    int charsPerRow;
    bool isLoaded;

    public:
    FontRenderer();
    ~FontRenderer();

    bool load(SDL_Renderer* renderer, const char* filename);
    void draw(SDL_Renderer* renderer, int x, int y, const char* text);
    void drawScaled(SDL_Renderer* renderer, int x, int y, const char* text, int scale);
    bool loaded() { return isLoaded; }
    int textWidth(const char* text, int scale = 1);
    SDL_Texture* getTexture() { return texture; }
};

#endif