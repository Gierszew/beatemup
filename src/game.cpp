#include "game.h"

Game::Game() {
    window = NULL;
    renderer = NULL;
    player = NULL;
    camera = NULL;
    background = nullptr;
    layerCount = 0;
    playStartTime = 0;
    debugMode = false;
}

bool Game::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return false;
    }

    this->width = width;
    this->height = height;
    this->worldWidth = 5000;
    this->worldHeight = 2000;
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

    background = nullptr;

    initBackground();
    
    player = new Player();
    player->load(1, 200, 32, 64, (char*)"assets/mage.bmp");
    player->setTexture(renderer);

    camera = new Camera(width, height);

    spawnEnemies();

    this->worldBounds = WorldBounds(0, 0, worldWidth, worldHeight);

    if (!font.load(renderer, "assets/cs8x8.bmp")) {
        printf("Failed to load font renderer.\n");
    }

    startTime = SDL_GetTicks();
    lastTick = SDL_GetTicks();
    deltaTime = 0.0f;

    menu = new Menu(renderer, &font);
    currentState = STATE_MENU;

    isRunning = true;
    return true;
}

SDL_Texture* Game::loadTexture(const char* filePath) {
    return SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP(filePath));
}

void Game::resetGame() {
    player->hp = 100;
    player->x = 1;
    player->y = 200;
    player->state = IDLE;
    player->score = 0;
    player->getInputBuffer()->clear();
    startTime = SDL_GetTicks();
    playStartTime = SDL_GetTicks();
    lastTick = SDL_GetTicks();
    deltaTime = 0.0f;
    currentLevel = 1;
    loadLevel(currentLevel);
}

void Game::renderHUD() {
    if (!font.loaded()) return;
    
    Uint32 elapsed = SDL_GetTicks() - playStartTime;
    char timeStr[32];
    sprintf(timeStr, "TIME: %02d:%02d", elapsed/60000, (elapsed/1000)%60);
    
    char reqStr[64];
    sprintf(reqStr, "REQ: 1-5, A-G");
    
    char posStr[48];
    sprintf(posStr, "POS: %.0f,%.0f", player->x, player->y);

    char scoreStr[32];
    sprintf(scoreStr, "SCORE: %d", player->score);
    
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    font.drawScaled(renderer, 10, 10, timeStr, 2);
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    font.drawScaled(renderer, 10, 40, reqStr, 2);
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    font.drawScaled(renderer, 10, 60, posStr, 1);

    SDL_SetRenderDrawColor(renderer, 0, 215, 0, 255);
    font.drawScaled(renderer, 10, 100, scoreStr, 1);
    
    SDL_Rect healthBar = { (int)((player->x) - camera->getViewPort().x), (int)(player->y - camera->getViewPort().y), player->hp * 2, 20 };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &healthBar);

    char attackStr[64];
    const char* stateNames[] = {"IDLE", "WALKING", "LIGHT ATK", "HEAVY ATK", "HURT", "DASHING"};
    
    sprintf(attackStr, "STATE: %s", stateNames[player->state]);
    
    SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
    font.drawScaled(renderer, 10, 80, attackStr, 1);
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }
        else if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
            if (currentState == STATE_MENU) {
                if (event.key.keysym.sym == SDLK_UP) {
                    menu->previousOption();
                }
                else if (event.key.keysym.sym == SDLK_DOWN) {
                    menu->nextOption();
                }
                else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    isRunning = false;
                }
                if (event.key.keysym.sym == SDLK_RETURN) {
                    int choice = menu->getSelectedOption();
                    if (choice == 0 || choice == 1) {
                        currentLevel = 1;
                    } else if (choice == 2) {
                        currentLevel = 2;
                    }

                    loadLevel(currentLevel);
                    playStartTime = SDL_GetTicks();
                    currentState = STATE_PLAYING;
                }
                continue;
            }

            else if (currentState == STATE_GAMEOVER) {
                if (event.key.keysym.sym == SDLK_m) {
                    currentState = STATE_MENU;
                } else if (event.key.keysym.sym == SDLK_r) {
                    printf("Restarting game...\n");
                    resetGame();
                    currentState = STATE_PLAYING;
                }
                continue;
            } else if (currentState == STATE_PLAYING) {
                InputKey key = InputBuffer::scancodeToInput((SDL_Scancode)event.key.keysym.scancode);
                if (key != KEY_NONE) {
                    float currentTime = (SDL_GetTicks() - startTime) / 1000.0f;
                    player->getInputBuffer()->addInput(key, currentTime);
                    player->checkCombos(currentTime);
                }
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        isRunning = false;
                        break;
                    case SDLK_n:
                        resetGame();
                        break;
                    case SDLK_F3:
                        debugMode = !debugMode;
                        break;
                }
            }
        }
    }
}

void Game::update() {
    if (currentState != STATE_PLAYING) {
        return;
    }

    if (player->hp <= 0) {
        currentState = STATE_GAMEOVER;
    }

    Uint32 currentTick = SDL_GetTicks();
    deltaTime = (currentTick - lastTick) / 1000.0f;
    lastTick = currentTick;

    player->update(deltaTime);

    worldBounds.clampPlayer(&player->x, &player->y, player->width, player->height);

    float requiredZoom = 1.0f / player->scale;
    camera->setZoom(requiredZoom);
    int visualCenterX = (int)(player->x + player->width / 2);
    int visualCenterY = (int)(player->y + player->height / 2);
    camera->update(visualCenterX, visualCenterY, worldBounds.right, worldBounds.bottom);

    updateEnemies(deltaTime);
    checkCollisions();

    bool allDead = true;
    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i] && enemies[i]->isAlive()) {
            allDead = false;
            break;
        }
    }

    if (allDead) {
        if (currentLevel < MAX_LEVELS) {
            currentLevel++;
            loadLevel(currentLevel);
            printf("Loading level %d\n", currentLevel);
        } else {
            currentState = STATE_MENU;
            currentLevel = 1; 
        }
    }
}

void Game::renderDebugInfo() {
    if (!debugMode) return;

    SDL_Rect debugRect = { 500, 10, 280, 220 };
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_RenderFillRect(renderer, &debugRect);

    const char* stateNames[] = {"IDLE", "WALKING", "LIGHT ATK", "HEAVY ATK", "HURT", "DASHING", "DEAD"};
    char actionStr[64];
    sprintf(actionStr, "ACTION: %s", stateNames[player->state]);
    font.drawScaled(renderer, 510, 20, actionStr, 1);

    font.drawScaled(renderer, 510, 45, "INPUT BUFFER (Newest first):", 1);
    
    int bufSize = player->getInputBuffer()->getBufferSize();
    int yOffset = 70;

    for (int i = 0; i < bufSize; i++) {
        InputEvent ev = player->getInputBuffer()->getEvent(i);
        
        char inputStr[64];
        const char* keyName = player->getInputBuffer()->keyToString(ev.key);
        
        sprintf(inputStr, "[%.2f] %s %s", ev.timestamp, keyName, ev.consumed ? "*" : "");
        
        if (ev.consumed) {
            SDL_SetTextureColorMod(font.getTexture(), 100, 100, 100);
        } else {
            SDL_SetTextureColorMod(font.getTexture(), 255, 255, 0);
        }

        font.drawScaled(renderer, 515, yOffset, inputStr, 1);
        yOffset += 15;
    }
    
    SDL_SetTextureColorMod(font.getTexture(), 255, 255, 255);
}

void Game::initBackground() {
    layerCount = 1;
    backgroundLayers[0] = BackgroundLayer(worldBounds.right, worldBounds.bottom, false);
    backgroundLayers[0].load(renderer, (char*)"assets/bg.bmp", 0.05f, false);
}

void Game::renderBackground() {
    SDL_Rect viewPort = camera->getViewPort();
    for (int i = 0; i < layerCount; i++) {
        backgroundLayers[i].update(viewPort.x, viewPort.y);
        backgroundLayers[i].draw(renderer, viewPort, camera->zoom);
    }
}

void Game::loadLevel(int levelNum) {
    if (enemies) {
        for (int i = 0; i < enemyCount; i++) delete enemies[i];
        free(enemies);
        enemies = nullptr;
    }
    char levelPath[64];
    sprintf(levelPath, "levels/level%d.txt", levelNum);
    LevelLoader loader;
    if (!loader.load(levelPath, this)) {
        printf("Błąd wczytywania poziomu: %s\n", levelPath);
        currentState = STATE_MENU;
    }
    
    player->x = 100;
    player->y = 200;
}

void Game::spawnEnemies() {
    LevelLoader loader;
    if (!loader.load("levels/level1.txt", this)) {
        printf("BŁĄD: Nie udalo sie wczytac poziomu!\n");
        enemyCount = 0; 
    }
}
void Game::updateEnemies(float dt) {
    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i] && enemies[i]->isAlive()) {
            enemies[i]->update(dt);
        }
    }
}

void Game::renderEnemies() {
    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i] && enemies[i]->isAlive()) {
            enemies[i]->draw(renderer, camera, debugMode);
            char hpStr[16];
            sprintf(hpStr, "HP: %d", enemies[i]->getHealth());
            font.drawScaled(renderer, (int)(enemies[i]->x - camera->getViewPort().x), 
                            (int)(enemies[i]->y - camera->getViewPort().y - 20), hpStr, 1);
        }
    }
}

void Game::checkCollisions() {
    float yTolerance = 200.0f;

    if (player->isAttacking()) {
        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i]->isAlive()) {
                Hitbox playerAttack = player->getAttackHitbox();
                Hitbox enemyBody = enemies[i]->getBodyHitbox();

                if (playerAttack.collidesWithDepth(player->x, player->y, enemyBody, enemies[i]->x, enemies[i]->y, yTolerance)) {
                    enemies[i]->takeDamage(player->getAttackDamage());
                }
            }
        }
    }

    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i]->isAttacking() && enemies[i]->isAlive()) {
            Hitbox enemyAttack = enemies[i]->getAttackHitbox();
            Hitbox playerBody = player->getBodyHitbox();

            if (enemyAttack.collidesWith(enemies[i]->x, enemies[i]->y, playerBody, player->x, player->y)) {
                player->takeDamage(enemies[i]->getDamage());
            }
        }
    }

    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i]->isAlive()) {
                if (player->collidesWith(enemies[i])) {
                    resolveCollisions(player, enemies[i]);
                }
        }
    }
}

void Game::resolveCollisions(GameObject* obj1, GameObject* obj2) {
    float dx = obj1->x - obj2->x;
    float dy = obj1->y - obj2->y;
    
    float distance = sqrt(dx*dx + dy*dy);
    
    if (distance == 0) {
        dx = 1.0f;
        dy = 0.0f;
        distance = 1.0f;
    }
    
    dx /= distance;
    dy /= distance;
    
    float minDistance = (obj1->getBodyHitbox().width/2 + obj2->getBodyHitbox().width/2) * 0.9f;
    
    if (distance < minDistance) {
        float overlap = minDistance - distance;
        
        obj1->setPosition(
            obj1->x + dx * overlap,
            obj1->y + dy * overlap
        );
        
        obj2->setPosition(
            obj2->x - dx * overlap * 0.5f,
            obj2->y - dy * overlap * 0.5f
        );
    }
}


void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (currentState == STATE_MENU) {
        menu->draw();
    } else if (currentState == STATE_GAMEOVER) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
        SDL_RenderClear(renderer);
        
        font.drawScaled(renderer, 250, 200, "GAME OVER", 4);
        
        char scoreStr[32];
        sprintf(scoreStr, "SCORE: %d", player->score);
        font.drawScaled(renderer, 300, 300, scoreStr, 2);
        
        font.drawScaled(renderer, 200, 400, "PRESS R TO RESTART OR M FOR MENU", 1);
    } else {

        SDL_Rect viewPort = camera->getViewPort();

        renderBackground();
        
        player->draw(renderer, camera, debugMode);
        renderEnemies();

        drawComboUI();

        renderHUD();
        if (debugMode) {
            renderDebugInfo();
        }
    }
        SDL_RenderPresent(renderer);
}

void Game::drawComboUI() {
    ComboType currentCombo = player->getCurrentCombo();

    if (currentCombo != COMBO_NONE) {
        char comboText[64];
        snprintf(comboText, sizeof(comboText), "COMBO: %s", 
                 player->getComboName(currentCombo));
        
        font.drawScaled(renderer, 400, 50, comboText, 2);
        
        SDL_Rect comboRect = {350, 45, 300, 60};
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 100);
        SDL_RenderFillRect(renderer, &comboRect);
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
        SDL_RenderDrawRect(renderer, &comboRect);
    }

    if (player->getComboMultiplier() > 1.0f) {
            char multiplierText[32];
            snprintf(multiplierText, sizeof(multiplierText), "x%.1f", 
                     player->getComboMultiplier());
            font.drawScaled(renderer, 500, 100, multiplierText, 3);
    }
}

Game::~Game() {
    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i]) delete enemies[i];
    }

    if (enemies) free(enemies);

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