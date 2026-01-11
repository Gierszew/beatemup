#include "game.h"

Game::Game() {
    window = NULL;
    renderer = NULL;
    player = NULL;
    camera = NULL;
    background = nullptr;
    layerCount = 0;
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
    player->load(1, worldHeight, 32, 64, (char*)"assets/mage.bmp");
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

    isRunning = true;
    return true;
}

SDL_Texture* Game::loadTexture(const char* filePath) {
    return SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP(filePath));
}

void Game::resetGame() {
    player->x = 100;
    player->y = 100;
    startTime = SDL_GetTicks();
    deltaTime = 0.0f;
}

void Game::renderHUD() {
    if (!font.loaded()) return;
    
    // 1. Czas gry
    Uint32 elapsed = SDL_GetTicks() - startTime;
    char timeStr[32];
    sprintf(timeStr, "TIME: %02d:%02d", elapsed/60000, (elapsed/1000)%60);
    
    // 2. Wymagania
    char reqStr[64];
    sprintf(reqStr, "REQ: 1-3[OK] 4[HUD]");
    
    // 3. Pozycja (debug)
    char posStr[48];
    sprintf(posStr, "POS: %.0f,%.0f", player->x, player->y);
    
    // 4. Rysuj (SCREEN coordinates!)
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    font.drawScaled(renderer, 10, 10, timeStr, 2);
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    font.drawScaled(renderer, 10, 40, reqStr, 1);
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    font.drawScaled(renderer, 10, 60, posStr, 1);
    
    // 5. Pasek życia (przykład)
    char hpStr[32] = "HP: [====    ]";
    SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
    font.drawScaled(renderer, width - 150, 10, hpStr, 2);

    char attackStr[64];
    const char* stateNames[] = {"IDLE", "WALKING", "LIGHT ATK", "HEAVY ATK", "HURT"};
    
    sprintf(attackStr, "ATK: %s (J/K)", stateNames[player->state]);
    
    SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
    font.drawScaled(renderer, 10, 80, attackStr, 1);
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    isRunning = false;
                    break;
                case SDLK_n:
                    resetGame();
                    break;
            }
        }
    }
}

void Game::update() {
    // Update game logic here
    Uint32 currentTick = SDL_GetTicks();
    deltaTime = (currentTick - lastTick) / 1000.0f;
    lastTick = currentTick;

    player->update(deltaTime);

    worldBounds.clampPlayer(&player->x, &player->y, player->width, player->height);

    float requiredZoom = 1.0f / player->scale;
    camera->setZoom(requiredZoom);
    int visualCenterX = (int)(player->x + player->width / 2);
    int visualCenterY = (int)(player->y + player->height / 2);
    camera->update(visualCenterX, visualCenterY, worldWidth, worldHeight);

    updateEnemies(deltaTime);
    checkCollisions();

    printf("Player: scale=%.1f world=(%.0f,%.0f) size=%dx%d center=(%d,%d)\n",
           player->scale, player->x, player->y, player->width, player->height,
           visualCenterX, visualCenterY);
}

void Game::initBackground() {
    layerCount = 1;
    backgroundLayers[0].load(renderer, (char*)"assets/bg.bmp", 0.05f, false);
}

void Game::renderBackground() {
    SDL_Rect viewPort = camera->getViewPort();
    for (int i = 0; i < layerCount; i++) {
        backgroundLayers[i].update(viewPort.x, viewPort.y);
        backgroundLayers[i].draw(renderer, viewPort, camera->zoom);
    }
}

void Game::spawnEnemies() {
    maxEnemies = 5;
    enemyCount = 2;

    enemies = (Enemy**)malloc(sizeof(Enemy*) * maxEnemies);

    enemies[0] = new Enemy();
    enemies[0]->init(CHASER, 400, 400, player);
    enemies[0]->setTexture(renderer);

    enemies[1] = new Enemy();
    enemies[1]->init(RANGED, 800, 400, player);
    enemies[1]->setTexture(renderer);

    printf("Spawned %d enemies.\n", enemyCount);
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
            enemies[i]->draw(renderer, camera);
            printf("Rendering Enemy %d at (%.1f, %.1f)\n", i, enemies[i]->x, enemies[i]->y);
        } else {
            printf("Enemy %d is not alive, skipping render.\n", i);
        }
    }
}

void Game::checkCollisions() {
    if (player->isAttacking()) {
        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i]->isAlive()) {
                Hitbox playerAttack = player->getAttackHitbox();
                Hitbox enemyBody = enemies[i]->getBodyHitbox();

                if (playerAttack.collidesWith(player->x, player->y, enemyBody, enemies[i]->x, enemies[i]->y)) {
                    printf("Player attack hit Enemy %d!\n", i);
                    enemies[i]->takeDamage(4);  // PLAYER'S ATTACK DAMAGE
                }
            }
        }
    }

    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i]->isAttacking() && enemies[i]->isAlive()) {
            Hitbox enemyAttack = enemies[i]->getAttackHitbox();
            Hitbox playerBody = player->getBodyHitbox();

            if (enemyAttack.collidesWith(enemies[i]->x, enemies[i]->y, playerBody, player->x, player->y)) {
                printf("Enemy %d attack hit Player!\n", i);
                //player->takeDamage(enemies[i]->getDamage());
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

void Game::resolveCollisions(Player* player, Enemy* enemy) {
    // Oblicz wektor od przeciwnika do gracza
    float dx = player->x - enemy->x;
    float dy = player->y - enemy->y;
    
    // Długość wektora
    float distance = sqrt(dx*dx + dy*dy);
    
    if (distance == 0) {
        // Jeśli dokładnie w tym samym miejscu, odsuń losowo
        dx = 1.0f;
        dy = 0.0f;
        distance = 1.0f;
    }
    
    // Normalizuj wektor
    dx /= distance;
    dy /= distance;
    
    // Minimalna odległość między hitboxami
    float minDistance = (player->getBodyHitbox().width/2 + enemy->getBodyHitbox().width/2) * 0.9f;
    
    if (distance < minDistance) {
        // Oblicz potrzebne przesunięcie
        float overlap = minDistance - distance;
        
        // Odsuń gracza
        player->setPosition(
            player->x + dx * overlap,
            player->y + dy * overlap
        );
        
        // Możesz też odsunąć przeciwnika w przeciwnym kierunku
        enemy->setPosition(
            enemy->x - dx * overlap * 0.5f,
            enemy->y - dy * overlap * 0.5f
        );
        
        printf("Collision resolved: player moved by (%.1f, %.1f)\n", 
               dx * overlap, dy * overlap);
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render game objects here

    SDL_Rect viewPort = camera->getViewPort();

    renderBackground();
    
    player->draw(renderer, camera);
    renderEnemies();

    renderHUD();
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