#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include "fontrender.h"

class Menu {
    private:
    SDL_Renderer* renderer;
    FontRenderer* font;
    int selectedOption;
    int totalOptions;

    public:
    Menu(SDL_Renderer* r, FontRenderer* f);
    void draw();
    void nextOption() {
        selectedOption = (selectedOption + 1) % totalOptions;
    }
    void previousOption() {
        selectedOption = (selectedOption - 1 + totalOptions) % totalOptions;
    }
    int getSelectedOption() const {
        return selectedOption;
    }
};

#endif