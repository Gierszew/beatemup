#include "player.h"

const ComboDefinition Player::comboDefinitions[] = {
    {COMBO_TRIPLE_LIGHT, "Triple Light", 
     {KEY_ATTACK_LIGHT, KEY_ATTACK_LIGHT, KEY_ATTACK_LIGHT}, 3, 1.5f},
    
    // Triple Heavy: H-H-H
    {COMBO_TRIPLE_HEAVY, "Triple Heavy",
     {KEY_ATTACK_HEAVY, KEY_ATTACK_HEAVY, KEY_ATTACK_HEAVY}, 3, 2.0f},
    
    // Mixed Combo: L-H-L
    {COMBO_MIXED, "Mixed Combo",
     {KEY_ATTACK_LIGHT, KEY_ATTACK_HEAVY, KEY_ATTACK_LIGHT}, 3, 2.5f},
    
    // Dash Left: ←←
    {COMBO_DASH_LEFT, "Dash Left",
     {KEY_LEFT, KEY_LEFT}, 2, 0.0f},  // No damage, just movement
     
    // Dash Right: →→  
    {COMBO_DASH_RIGHT, "Dash Right",
     {KEY_RIGHT, KEY_RIGHT}, 2, 0.0f},
     
    // Uppercut: ↓→ + L (jak w Street Fighter)
    {COMBO_UPPERCUT, "Uppercut",
     {KEY_DOWN, KEY_RIGHT, KEY_ATTACK_LIGHT}, 3, 3.0f}
};

Player::Player() {
    attackTimer = 0.0f;
    attackCooldown = 0.0f;
    state = IDLE;
    hp = 10;
    currentCombo = COMBO_NONE;
    comboCooldown = 0.0f;
    showDebugInfo = false;

    dashDirection = 1;
    dashTimer = 0.0f;
    dashCooldown = 0.0f;
    dashSpeed = 1200.0f;      // Prędkość podczas dasha
    normalSpeed = 500.0f;     // Normalna prędkość
    speed = normalSpeed;      // Aktualna prędkość = normalna
    dashOriginalSpeed = normalSpeed;
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
    GameObject::drawDebug(renderer, camera);

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
    if (dashTimer > 0) {
        dashTimer -= dt;
        
        // Podczas dasha poruszaj się
        if (state == DASHING) {
            x += speed * dashDirection * dt;
            
            // Clampowanie
            if (x < 0) x = 0;
            // if (x + width > worldWidth) x = worldWidth - width;
            
            /* printf("DASHING: x=%.1f, speed=%.0f, timer=%.3f\n", 
                   x, speed, dashTimer); */
        }
        
        // Koniec dasha
        if (dashTimer <= 0 && state == DASHING) {
            /* printf("DASH END: restoring speed from %.0f to %.0f\n", 
                   speed, dashOriginalSpeed); */
            
            // PRZYWRÓĆ normalną prędkość!
            speed = 500.0f;
            state = IDLE;
        }
    }
    
    inputBuffer.debugPrint(dt);

    // 2. Zmniejsz cooldown dasha
    if (dashCooldown > 0) {
        dashCooldown -= dt;
    }
    
    // 3. Normalny ruch TYLKO jeśli nie dashujemy
    if (state != DASHING && state != HURT) {
        handleInput(dt);
    }

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

    updateCombo(dt);

    if (showDebugInfo && currentCombo != COMBO_NONE) {
        // printf("Active Combo: %s\n", getComboName(currentCombo));
    }
}

void Player::handleInput(float dt) {
    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    handleMovement(keystate, dt);    
}

void Player::handleMovement(const Uint8* keystate, float currentTime) {
    static bool left = false;
    bool isMoving = false;

    if (state == DASHING || state == HURT) return;

    InputKey dashDir;

    if (dashCooldown <= 0 && state != DASHING) {
        if (inputBuffer.isDashing(currentTime, &dashDir)) {
            if (dashDir == KEY_LEFT) {
                performDash(true);
                return; // WAŻNE: wyjdź z funkcji po dash
            } else if (dashDir == KEY_RIGHT) {
                performDash(false);
                return; // WAŻNE: wyjdź z funkcji po dash
            }
        }
    }

    if (!isAttacking() && state != HURT && state != DASHING) {
        if (keystate[SDL_SCANCODE_W]) {
            y -= speed * currentTime;
            left = false;
            isMoving = true;
        }
        if (keystate[SDL_SCANCODE_S]) {
            y += speed * currentTime;
            left = false;
            isMoving = true;
        }
        if (keystate[SDL_SCANCODE_A]) {
            x -= speed * currentTime;
            left = true;
            isMoving = true;
        }
        if (keystate[SDL_SCANCODE_D]) {
            x += speed * currentTime;
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

    // printf("Players [X Y] [W H] [%f %f] [%d %d] \n", x, y, width, height);

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

int Player::getAttackDamage() const {
    int baseDamage = 0;
        if (state == ATTACKING_LIGHT) baseDamage = 1;
        if (state == ATTACKING_HEAVY) baseDamage = 2;
        
        // Zastosuj mnożnik combo
        return (int)(baseDamage * comboDamageMultiplier);
}

void Player::checkCombos(float currentTime) {
    if (comboCooldown > 0 || state == HURT) return;

    

    for (int i = 0; i < MAX_COMBOS; i++) {
        const ComboDefinition& combo = comboDefinitions[i];

        if (inputBuffer.checkSequence(combo.sequence, combo.length, currentTime)) {
            executeCombo(combo.type);
            currentCombo = combo.type;
            comboCooldown = 0.5f;

            /* printf("COMBO EXECUTED: %s! (x%.1f damage)\n", 
                   combo.name, combo.damageMultiplier); */
            return;
        }
    }

    currentCombo = COMBO_NONE;
}

void Player::executeCombo(ComboType combo) {
    for (int i = 0; i < MAX_COMBOS; i++) {
        if (comboDefinitions[i].type == combo) {
            comboDamageMultiplier = comboDefinitions[i].damageMultiplier;
            comboMultiplierTimer = 3.0f;  // Mnożnik aktywny przez 3 sekundy
            break;
        }
    }

    switch (combo) {
        case COMBO_TRIPLE_LIGHT:
            // 3 szybkie ataki
            state = ATTACKING_LIGHT;
            attackTimer = LIGHT_ATTACK_TIME * 0.7f;  // Szybsze
            // Ustaw większy hitbox
            setAttackHitbox(movingLeft ? -80 : width, height * 0.25f, 80, height * 0.5f);
            break;
            
        case COMBO_TRIPLE_HEAVY:
            // 3 mocne ataki
            state = ATTACKING_HEAVY;
            attackTimer = HEAVY_ATTACK_TIME * 0.8f;
            // Bardzo duży hitbox
            setAttackHitbox(movingLeft ? -100 : width, height * 0.2f, 100, height * 0.6f);
            break;
            
        case COMBO_MIXED:
            // Mieszany combo
            state = ATTACKING_HEAVY;  // Ostatni atak jest ciężki
            attackTimer = HEAVY_ATTACK_TIME;
            // Duży hitbox z knockback
            setAttackHitbox(movingLeft ? -90 : width, height * 0.15f, 90, height * 0.7f);
            break;
            
        case COMBO_DASH_LEFT:
            performDash(true);
            break;
            
        case COMBO_DASH_RIGHT:
            performDash(false);
            break;
            
        case COMBO_UPPERCUT:
            performUppercut();
            break;
            
        default:
            break;
    }

    inputBuffer.clear();
}

void Player::performDash(bool left) {
    if (state == DASHING || state == HURT || dashCooldown > 0 || state == ATTACKING_HEAVY || state == ATTACKING_LIGHT) return;  // Nie dashuj podczas dasha lub obrażeń
    
    // printf("DASH START: speed before=%.0f\n", speed);
    
    dashOriginalSpeed = speed;
    
    // Ustaw parametry dasha
    state = DASHING;
    currentRow = 3;
    dashDirection = left ? -1 : 1;
    dashTimer = 0.02f;
    dashCooldown = 1.5f;
    
    // Ustaw prędkość dasha (TYLKO na czas dasha)
    speed = dashSpeed;
    
    /* printf("DASH %s! Timer=%.2f, Speed=%.0f->%.0f\n", 
           left ? "LEFT" : "RIGHT", dashTimer, dashOriginalSpeed, speed); */
}

void Player::performUppercut() {
    state = ATTACKING_HEAVY;
    attackTimer = HEAVY_ATTACK_TIME * 1.2f;

    setAttackHitbox(width * 0.25f, -height * 0.5f, width * 0.5f, height * 0.8f);

    y -= 30.0f;

    // printf("UPPERCUT performed!\n");
}

void Player::updateCombo(float dt) {
    if (comboCooldown > 0) comboCooldown -= dt;

    inputBuffer.clearOldInputs(SDL_GetTicks() / 1000.0f);

    if (attackTimer <= 0 && currentCombo != COMBO_NONE) currentCombo = COMBO_NONE;

    if (comboMultiplierTimer > 0) {
        comboMultiplierTimer -= dt;
        if (comboMultiplierTimer <= 0) {
            comboDamageMultiplier = 1.0f;
        }
    }
}

const char* Player::getComboName(ComboType combo) const {
    if (combo >= 0 && combo < MAX_COMBOS) {
        return comboDefinitions[combo].name;
    }
    return "None";
}

