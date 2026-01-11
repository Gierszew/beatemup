#include "player.h"

Player::Player() {
    attackTimer = 0.0f;
    attackCooldown = 0.0f;
    state = IDLE;
    hp = 10;
}

void Player::load(int x, int y, int width, int height, char* textureId) {
    this->speed = 500.0f;
    this->scale = 5.0f;
    
    this->originalWidth = width;
    this->originalHeight = height;

    hp = 10;

    GameObject::load(x, y - (height * scale - height), width * scale, height * scale, textureId);

    float bodyWidth = width * 0.6f;
    float bodyHeight = height * 0.8f;
    float bodyOffsetX = (width - bodyWidth) / 2.0f;
    float bodyOffsetY = height - bodyHeight;

    setBodyHitbox(bodyOffsetX, bodyOffsetY, bodyWidth, bodyHeight);

    attackHitbox = Hitbox(0, 0, 0, 0); // Początkowo brak hitboxa ataku
}

void Player::setTexture(SDL_Renderer* renderer) {
    GameObject::setTexture(renderer);
}

void Player::draw(SDL_Renderer* renderer, Camera* camera) {
    GameObject::draw(renderer, camera, movingLeft);

    if (isAttacking()) {
        SDL_Rect viewPort = camera->getViewPort();
        int attackRange;
        if (state == ATTACKING_LIGHT) {
            attackRange = 40;
        } else if (state == ATTACKING_HEAVY) {
            attackRange = 70;
        } else {
            attackRange = 0;
        }
        int attackHeight = height / 2;

        SDL_Rect attackBox = {
            (int)x - viewPort.x + (movingLeft ? -attackRange : width),
            (int)y - viewPort.y + height/4,
            attackRange,
            attackHeight
        };
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128);
        SDL_RenderFillRect(renderer, &attackBox);
    }
}

void Player::update(float dt) {
    this->handleInput(dt);

    if (attackTimer > 0) {
        attackTimer -= dt;
        if (attackTimer <= 0) {
            attackCooldown = ATTACK_COOLDOWN;
            state = IDLE;
        }
    }

    if (attackCooldown > 0) {
        attackCooldown -= dt;
    }

    switch (state) {
        case ATTACKING_LIGHT:
            currentRow = 2;
            static float lightAnimTimer = 0;
            lightAnimTimer += dt;
            if (lightAnimTimer > LIGHT_ATTACK_TIME / 3) {
                lightAnimTimer = 0;
                currentFrame = (currentFrame + 1) % 3;
            }  // Klatka lekkiego ataku
            break;
        case ATTACKING_HEAVY:
            currentRow = 4;
            static float heavyAnimTimer = 0;
            heavyAnimTimer += dt;
            if (heavyAnimTimer > HEAVY_ATTACK_TIME / 4) {
                heavyAnimTimer = 0;
                currentFrame = (currentFrame + 1) % 4;
            }
            break;
        case WALKING:
            // Prosta animacja chodzenia
            currentRow = 0;
            static float walkAnimTimer = 0;
            walkAnimTimer += dt;
            if (walkAnimTimer > 0.15f) {
                walkAnimTimer = 0;
                currentFrame = (currentFrame + 1) % 7;
            }
            break;
        case HURT:
            currentRow = 1;
            currentFrame = 3;  // Klatka obrażeń
            break;
        default:
            currentRow = 0;
            currentFrame = 0;  // Klatka spoczynkowa
    }

    if (attackTimer <= 0 && state != IDLE && state != WALKING) {
        state = IDLE;

        setAttackHitbox(0, 0, 0, 0);
    }
}

void Player::handleInput(float dt) {
    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    static bool left = false;
    bool isMoving = false;

    if (!isAttacking()) {
        if (keystate[SDL_SCANCODE_W]) {
            y -= speed * dt;
            left = false;
            isMoving = true;
        }
        if (keystate[SDL_SCANCODE_S]) {
            y += speed * dt;
            left = false;
            isMoving = true;
        }
        if (keystate[SDL_SCANCODE_A]) {
            x -= speed * dt;
            left = true;
            isMoving = true;
        }
        if (keystate[SDL_SCANCODE_D]) {
            x += speed * dt;
            left = false;
            isMoving = true;
        }

        if (keystate[SDL_SCANCODE_J] && attackCooldown <= 0) {
            attackLight();
        }
        if (keystate[SDL_SCANCODE_K] && attackCooldown <= 0) {
            attackHeavy();
        }
    }

    if (state == IDLE || state == WALKING) {
        state = isMoving ? WALKING : IDLE;
    }

    this->movingLeft = left;

    printf("Players [X Y] [W H] [%f %f] [%d %d] \n", x, y, width, height);

    if (y + height > worldHeight - 30) {
        y = worldHeight - height - 30;
    }
    if (y <= 54)
        y = 54;
}

void Player::attackLight() {
    state = ATTACKING_LIGHT;
    attackTimer = LIGHT_ATTACK_TIME;

    float attackWidth = 60.0f;
    float attackHeight = height / 2.0f;
    float attackOffsetX = movingLeft ? -attackWidth : width;
    float attackOffsetY = height / 4.0f;

    setAttackHitbox(attackOffsetX, attackOffsetY, attackWidth, attackHeight);
}

void Player::attackHeavy() {
    state = ATTACKING_HEAVY;
    attackTimer = HEAVY_ATTACK_TIME;

    float attackWidth = 90.0f;
    float attackHeight = height * 0.6f;
    float attackOffsetX = movingLeft ? -attackWidth : width;
    float attackOffsetY = height / 5.0f;

    setAttackHitbox(attackOffsetX, attackOffsetY, attackWidth, attackHeight);
}