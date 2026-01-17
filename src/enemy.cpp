#include "enemy.h"
#include <cmath>

Enemy::Enemy() {
    state = PATROLLING;
    type = CHASER;
    target = nullptr;
    health = MAX_HEALTH;
    damage = 1;
    attackRange = ATTACK_DISTANCE;
    attackCooldown = 0.0f;
    stunTimer = 0.0f;
    scale = 5.0f;
    animTimer = 0.0f;
}

void Enemy::init(EnemyType type, float x, float y, Player* target) {
    this->type = type;
    this->x = x;
    this->y = y;
    this->target = target;
    health = MAX_HEALTH;
    state = PATROLLING;
    currentFrame = 0;
    currentRow = 0;

    this->originalWidth = 32;
    this->originalHeight = 64;

    char* textureFile = (char*)"assets/ninja.bmp";

    GameObject::load(x, y, originalWidth * scale, originalHeight * scale, textureFile);

    if (type == CHASER) {
        speed = CHASER_SPEED;
        damage = 20;
        attackRange = 50.0f;
    } else if (type == RANGED) {
        speed = RANGED_SPEED;
        damage = 10;
        attackRange = 200.0f;
    }

    float bodyWidth = 32 * 0.7f;
    float bodyHeight = 64 * 0.8f;
    float bodyOffsetX = (32 - bodyWidth) / 2.0f;
    float bodyOffsetY = 64 - bodyHeight;
    
    setBodyHitbox(bodyOffsetX, bodyOffsetY, bodyWidth, bodyHeight);
}

void Enemy::update(float dt) {
    if (!isAlive() || !target) return;

    if (invincibilityFrames > 0) invincibilityFrames -= dt;
    if (attackCooldown > 0) attackCooldown -= dt;

    if (state == ATTACKING) {
        attackTimer -= dt;
        if (attackTimer <= 0) {
            state = PATROLLING;
            attackCooldown = 1.0f;
            setAttackHitbox(0, 0, 0, 0);
        }
    }

    switch(state) {
        case STUNNED:
            stunTimer -= dt;
            if (stunTimer <= 0) {
                state = PATROLLING;
            }
            return;
        case PATROLLING:
        case CHASING:
        currentRow = 0;
            animTimer += dt;
            if (animTimer > 0.2f) {
                animTimer = 0;
                currentFrame = (currentFrame + 1) % 4;
            }
            break;
        case ATTACKING:
        currentRow = 2;
            static float lightAnimTimer = 0;
            lightAnimTimer += dt;
            if (lightAnimTimer > 0.5f / 3) {
                lightAnimTimer = 0;
                currentFrame = (currentFrame + 1) % 3;
            }
    }
    if (state != STUNNED && state != ATTACKING) {
        switch (type) {
            case CHASER: updateChaser(dt); break;
            case RANGED: updateRanged(dt); break;
        }
    }

    if (!isAlive()) {
        return;
    }
    
    if (!target) {
        return;
    }
}

void Enemy::updateChaser(float dt) {
    float dx = target->x - x;
    float dy = target->y - y;
    float distance = sqrt(dx*dx + dy*dy);
    
    if (distance < attackRange) {
        state = ATTACKING;
        attackPlayer();
        attackCooldown = 1.0f;
    } 
    else if (distance < 500.0f) {
        state = CHASING;
        if (distance > 0) {
            x += (dx / distance) * speed * dt;
            y += (dy / distance) * speed * dt;
        }
    }
    else {
        state = PATROLLING;
        x += speed * 0.3f * dt;
    }
}

void Enemy::updateRanged(float dt) {
    float dx = target->x - x;
    float dy = target->y - y;
    float distance = sqrt(dx*dx + dy*dy);
    
    switch(state) {
        case PATROLLING: {
            if (fabs(dy) < 50.0f) {
                state = CHASING;
            } else {
                if (dy > 0) y += speed * dt;
                else y -= speed * dt;
            }
            break;
        }
        case CHASING: {
            if (dx > 0) x += CHARGE_SPEED * dt;
            else x -= CHARGE_SPEED * dt;
            
            if (distance > 300.0f || (dx > 0 && target->x < x) || (dx < 0 && target->x > x)) {
                state = PATROLLING;
            }
            
            if (distance < attackRange) {
                state = ATTACKING;
                attackPlayer();
            }
            break;
        }
        default:
            break;
    }
}

void Enemy::attackPlayer() {
    state = ATTACKING;
    attackTimer = 0.5f;

    float attackWidth = 60.0f;
    float attackHeight = height / 2.0f;
    float attackOffsetX = movingLeft ? -attackWidth : width;
    float attackOffsetY = height / 4.0f;

    setAttackHitbox(attackOffsetX, attackOffsetY, attackWidth, attackHeight);
}

void Enemy::takeDamage(int damage) {
    if (!isAlive() || invincibilityFrames > 0) return;
    
    health -= damage;
    invincibilityFrames = 0.5f;
    
    if (health <= 0) {
        state = DEAD;
        target->score += 1 * target->getComboMultiplier();
    } else {
        state = STUNNED;
        stunTimer = STUN_TIME;
    }
}

void Enemy::draw(SDL_Renderer* renderer, Camera* camera, bool debug) {
    if (!isAlive() || !texture) return;
    
    // Kolor wg typu/stanu
    if (type == CHASER) {
        SDL_SetTextureColorMod(texture, 255, 100, 100); // Czerwony
    } else {
        SDL_SetTextureColorMod(texture, 100, 100, 255); // Niebieski
    }
    
    if (state == STUNNED) {
        SDL_SetTextureColorMod(texture, 255, 255, 100); // Żółty (stunned)
    }
    
    if (target->x < x) {
        movingLeft = true;
    } else {
        movingLeft = false;
    }

    GameObject::draw(renderer, camera, movingLeft);
    if (debug) {
        GameObject::drawDebug(renderer, camera);
    }
}