#include "game.h"

bool Game::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return false;
    }

    this->width = width;
    this->height = height;
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    background = loadTexture("endless.bmp");

    isRunning = true;
    return true;
}

SDL_Texture* Game::loadTexture(const char* filePath) {
    return SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP(filePath));
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }
    }
}

void Game::update() {
    // Update game logic here
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render game objects here
    
    if (background) {
        SDL_RenderCopy(renderer, background, NULL, NULL);
    }
    SDL_RenderPresent(renderer);
}

Game::~Game() {
    SDL_DestroyTexture(background);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::run() {
    while (isRunning) {
        handleEvents();
        update();
        render();
    }
}