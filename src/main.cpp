#include "game.h"

int main(int argc, char* argv[]) {
    setbuf(stdout, NULL);

    Game game;

    if (!game.init("My SDL2 Game", 1047, 720)) {
        return -1;
    }

    game.run();

    return 0;
}