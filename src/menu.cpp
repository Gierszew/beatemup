#include "menu.h"

Menu::Menu(SDL_Renderer* r, FontRenderer* f) : renderer(r), font(f), selectedOption(0), totalOptions(3) {}

void Menu::draw() {
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
    SDL_RenderClear(renderer);

    font->drawScaled(renderer, 150, 100, "BEATEM UP", 3);

    SDL_SetRenderDrawColor(renderer, (selectedOption == 0) ? 255 : 255, (selectedOption == 0) ? 255 : 255, (selectedOption == 0) ? 0 : 255, 255);
    font->drawScaled(renderer, 200, 250, selectedOption == 0 ? "> START GAME" : "  START GAME", 1);

    SDL_SetRenderDrawColor(renderer, (selectedOption == 1) ? 255 : 255, (selectedOption == 1) ? 255 : 255, (selectedOption == 1) ? 0 : 255, 255);
    font->drawScaled(renderer, 200, 300, selectedOption == 1 ? "> LEVEL 1" : "  LEVEL 1", 1);

    SDL_SetRenderDrawColor(renderer, (selectedOption == 2) ? 255 : 255, (selectedOption == 2) ? 255 : 255, (selectedOption == 2) ? 0 : 255, 255);
    font->drawScaled(renderer, 200, 350, selectedOption == 2 ? "> LEVEL 2" : "  LEVEL 2", 1);

    SDL_SetRenderDrawColor(renderer, (selectedOption == 3) ? 255 : 255, (selectedOption == 3) ? 255 : 255, (selectedOption == 3) ? 0 : 255, 255);
    font->drawScaled(renderer, 200, 400, selectedOption == 3 ? "> UNAVAILABLE" : "  UNAVAILABLE", 1);

    font->drawScaled(renderer, 200, 500, "USE ARROWS & ENTER", 1);
}