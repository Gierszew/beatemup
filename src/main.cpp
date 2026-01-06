#include "game.h"

int main(int argc, char* argv[]) {
    Game game;

    if (!game.init("My SDL2 Game", 800, 600)) {
        return -1;
    }

    game.run();

    return 0;
}