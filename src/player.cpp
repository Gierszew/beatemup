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
     {KEY_LEFT, KEY_LEFT}, 2, 0.0f},
     
    // Dash Right: →→  
    {COMBO_DASH_RIGHT, "Dash Right",
     {KEY_RIGHT, KEY_RIGHT}, 2, 0.0f},
     
    // Uppercut: ↓→ + L
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
    dashSpeed = 800.0f;
    normalSpeed = 500.0f;
    speed = normalSpeed;
    dashOriginalSpeed = normalSpeed;
    score = 0;
    this->comboDamageMultiplier = 1.0f;
    this->score = 0;
    this->currentCombo = COMBO_NONE;
}

void Player::load(int x, int y, int width, int height, char* textureId) {
    this->speed = 500.0f;
    this->scale = 5.0f;
    
    this->originalWidth = width;
    this->originalHeight = height;

    invincibilityFrames = 0.0f;

    hp = MAX_HP;
    score = 0;
    GameObject::load(x, y, width * scale, height * scale, textureId);

    float bodyWidth = width * 0.6f;
    float bodyHeight = height * 0.8f;
    float bodyOffsetX = (width - bodyWidth) / 2.0f;
    float bodyOffsetY = height - bodyHeight;

    setBodyHitbox(bodyOffsetX, bodyOffsetY, bodyWidth, bodyHeight);

    attackHitbox = Hitbox(0, 0, 0, 0);
}

void Player::setTexture(SDL_Renderer* renderer) {
    GameObject::setTexture(renderer);
}

void Player::draw(SDL_Renderer* renderer, Camera* camera, bool debug) {
    GameObject::draw(renderer, camera, movingLeft);
    if (debug) {
        GameObject::drawDebug(renderer, camera);
    }

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
    inputBuffer.debugPrint(dt);

    if (invincibilityFrames > 0) {
        invincibilityFrames -= dt;
    }

    if (dashCooldown > 0) {
        dashCooldown -= dt;
    }
    
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
            }
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
            currentRow = 0;
            static float walkAnimTimer = 0;
            walkAnimTimer += dt;
            if (walkAnimTimer > 0.15f) {
                walkAnimTimer = 0;
                currentFrame = (currentFrame + 1) % 7;
            }
            break;
        case DASHING:
            currentRow = 2;
            currentFrame = 5;
            static float dashAnimTimer = 0;
            dashAnimTimer += dt;
            if (dashAnimTimer > 0.1f) {
                printf("DASH ANIM FRAME UPDATE\n");
                dashAnimTimer = 0;
                currentFrame = (currentFrame + 1) % 2 + 5;
            }
            break;
        case HURT:
            currentRow = 1;
            currentFrame = 3;
            break;
        default:
            currentRow = 0;
            currentFrame = 0;
    }

    if (attackTimer <= 0 && state != IDLE && state != WALKING) {
        state = IDLE;

        setAttackHitbox(0, 0, 0, 0);
    }

    if (dashTimer > 0) {
        state = DASHING;
        dashTimer -= dt;
        x += speed * dashDirection * dt;
        
        if (x < 0) x = 0;
        
            
        if (dashTimer <= 0) {
            speed = 500.0f;
            state = IDLE;
        }
    }

    updateCombo(dt);

}

void Player::handleInput(float dt) {
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    handleMovement(keystate, dt);    
}

void Player::handleMovement(const Uint8* keystate, float currentTime) {
    static bool left = false;
    bool isMoving = false;

    if (state == DASHING || state == HURT) return;

    if (!isAttacking() && state != HURT && state != DASHING) {
        if (keystate[SDL_SCANCODE_W] && dashTimer <= 0) {
            y -= speed * currentTime;
            left = false;
            isMoving = true;
        }
        if (keystate[SDL_SCANCODE_S] && dashTimer <= 0) {
            y += speed * currentTime;
            left = false;
            isMoving = true;
        }
        if (keystate[SDL_SCANCODE_A] && dashTimer <= 0) {
            x -= speed * currentTime;
            left = true;
            isMoving = true;
        }
        if (keystate[SDL_SCANCODE_D] && dashTimer <= 0) {
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

            return;
        }
    }

    currentCombo = COMBO_NONE;
}

void Player::executeCombo(ComboType combo) {
    for (int i = 0; i < MAX_COMBOS; i++) {
        if (comboDefinitions[i].type == combo) {
            comboDamageMultiplier = comboDefinitions[i].damageMultiplier;
            comboMultiplierTimer = 3.0f;
            break;
        }
    }

    switch (combo) {
        case COMBO_TRIPLE_LIGHT:
            state = ATTACKING_LIGHT;
            attackTimer = LIGHT_ATTACK_TIME * 0.7f;
            setAttackHitbox(movingLeft ? -80 : width, height * 0.25f, 80, height * 0.5f);
            break;
            
        case COMBO_TRIPLE_HEAVY:
            state = ATTACKING_HEAVY;
            attackTimer = HEAVY_ATTACK_TIME * 0.8f;
            setAttackHitbox(movingLeft ? -100 : width, height * 0.2f, 100, height * 0.6f);
            break;
            
        case COMBO_MIXED:
            state = ATTACKING_HEAVY;
            attackTimer = HEAVY_ATTACK_TIME;

            setAttackHitbox(movingLeft ? -90 : width, height * 0.15f, 90, height * 0.7f);
            break;
            
        case COMBO_DASH_LEFT:
            state = DASHING;
            performDash(true);
            break;
            
        case COMBO_DASH_RIGHT:
            state = DASHING;
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
    printf("DASH CALLED! State before: %d\n", state);
    
    if (state == HURT) {
        printf("Cannot dash: HURT state\n");
        return;
    }
    if (dashCooldown > 0) {
        printf("Cannot dash: on cooldown (%.2f)\n", dashCooldown);
        return;
    }
    
    dashOriginalSpeed = speed;
    
    state = DASHING;
    dashDirection = left ? -1 : 1;
    dashTimer = 0.3f;
    dashCooldown = 1.0f;
    
    speed = dashSpeed;
}

void Player::performUppercut() {
    state = ATTACKING_HEAVY;
    attackTimer = HEAVY_ATTACK_TIME * 1.2f;

    setAttackHitbox(width * 0.25f, -height * 0.5f, width * 0.5f, height * 0.8f);

    y -= 30.0f;
}

void Player::updateCombo(float dt) {
    if (comboCooldown > 0) comboCooldown -= dt;

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

void Player::takeDamage(int damage) {
    if (state == HURT || invincibilityFrames > 0) return;

    hp -= damage;
    if (hp < 0) hp = 0;

    state = HURT;
    attackTimer = 0.5f;
    invincibilityFrames = 2.5f;
}