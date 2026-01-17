#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class Game;

class LevelLoader {
    public:

    int enemyCount;
    int worldWidth;
    int worldHeight;

    LevelLoader();

    int getValueAfterLabel(const char* line);
    bool load(const char* filePath, Game* game);;

};

#endif