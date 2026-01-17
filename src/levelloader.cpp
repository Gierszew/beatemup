#include "game.h"
#include "levelloader.h"

LevelLoader::LevelLoader() {
    enemyCount = 0;
    worldWidth = 0;
    worldHeight = 0;
}

int LevelLoader::getValueAfterLabel(const char* line) {
    const char* p = strchr(line, ':');
    if (p) return atoi(p + 1);
    return 0;
}

bool LevelLoader::load(const char* filePath, Game* game) {
    FILE* file = fopen(filePath, "r");
    if (!file) {
        printf("Failed to open level file: %s\n", filePath);
        return false;
    }

    char line[256];

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "Enemies:")) enemyCount = getValueAfterLabel(line);
        if (strstr(line, "WorldWidth:")) worldWidth = getValueAfterLabel(line);
        if (strstr(line, "[DATA]")) break;
    }

    game->enemyCount = this->enemyCount;
    game->enemies = (Enemy**)malloc(sizeof(Enemy*) * enemyCount);
    game->worldBounds.right = worldWidth;

    int eIdx = 0;
    int oIdx = 0;
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'E') {
            strtok(line, " ");
            int type = atoi(strtok(NULL, " "));
            int x = atoi(strtok(NULL, " "));
            int y = atoi(strtok(NULL, " "));
            printf("Loading Enemy: type=%d pos=(%d,%d)\n", type, x, y);
            
            if (eIdx < enemyCount) {
                game->enemies[eIdx] = new Enemy();
                game->enemies[eIdx]->init((EnemyType)type, x, y, game->player);
                game->enemies[eIdx]->setTexture(game->getRenderer());
                eIdx++;
            }
        }
    }

    fclose(file);
    return true;
}