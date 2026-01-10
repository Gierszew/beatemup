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
}

void Enemy::init(EnemyType type, float x, float y, Player* target) {
    this->type = type;
    this->x = x;
    this->y = y;
    this->target = target;
    health = MAX_HEALTH;
    state = PATROLLING;

    char* textureFile = "assets/ninja.bmp";

    GameObject::load(x, y, 32, 64, textureFile);

    if (type == CHASER) {
        speed = CHASER_SPEED;
        damage = 2;
        attackRange = 50.0f;
    } else if (type == RANGED) {
        speed = RANGED_SPEED;
        damage = 1;
        attackRange = 200.0f;
    }
}

void Enemy::update(float dt) {
    if (!isAlive() || !target) return;

    switch(state) {
        case STUNNED:
            stunTimer -= dt;
            if (stunTimer <= 0) {
                state = PATROLLING;
            }
            return;
    }

    switch (type) {
        case CHASER:
            updateChaser(dt);
            break;
        case RANGED:
            updateRanged(dt);
            break;
    }

    static float animTimer = 0;
    animTimer += dt;
    if (animTimer > 0.2f) {
        animTimer = 0;
        currentFrame = (currentFrame + 1) % 4;  // NO POWIEDZMY ŻE JEST 4
    }
}

void Enemy::updateChaser(float dt) {
    float dx = target->x - x;
    float dy = target->y - y;
    float distance = sqrt(dx*dx + dy*dy);
    
    // Jeśli blisko → atakuj
    if (distance < attackRange) {
        state = ATTACKING;
        printf("Chaser ATTACKING player!\n");
        // Tutaj: zadaj obrażenia graczowi
        // target->takeDamage(damage);
        attackCooldown = 1.0f;
    } 
    // Jeśli daleko → goni
    else if (distance < 500.0f) {
        state = CHASING;
        if (distance > 0) {
            x += (dx / distance) * speed * dt;
            y += (dy / distance) * speed * dt;
        }
    }
    // Jeśli bardzo daleko → patroluj
    else {
        state = PATROLLING;
        // Proste patrolowanie (możesz dodać)
        x += speed * 0.3f * dt;
    }
}

void Enemy::updateRanged(float dt) {
    float dx = target->x - x;
    float dy = target->y - y;
    float distance = sqrt(dx*dx + dy*dy);
    
    // RANGED: ustaw się w jednej linii, potem szarżuj
    if (state == PATROLLING) {
        // Sprawdź czy jest w linii poziomej z graczem
        if (fabs(dy) < 50.0f) {
            state = CHASING;
            printf("Ranged lining up for charge!\n");
        } else {
            // Porusz się w pionie aby się wyrównać
            if (dy > 0) y += speed * dt;
            else y -= speed * dt;
        }
    }
    else if (state == CHASING) {
        // Szarża!
        if (dx > 0) x += CHARGE_SPEED * dt;
        else x -= CHARGE_SPEED * dt;
        
        // Jeśli minął gracza lub zbyt daleko
        if (distance > 300.0f || (dx > 0 && target->x < x) || (dx < 0 && target->x > x)) {
            state = PATROLLING;
        }
        
        // Jeśli trafi w gracza podczas szarży
        if (distance < attackRange) {
            printf("Ranged CHARGED into player!\n");
            // target->takeDamage(damage);
            state = PATROLLING;
        }
    }
}

void Enemy::takeDamage(int damage) {
    if (!isAlive()) return;
    
    health -= damage;
    
    if (health <= 0) {
        state = DEAD;
        printf("Enemy DEFEATED!\n");
    } else {
        state = STUNNED;
        stunTimer = STUN_TIME;
        printf("Enemy STUNNED for %.1fs (HP: %d)\n", STUN_TIME, health);
    }
}

void Enemy::draw(SDL_Renderer* renderer, Camera* camera) {
    if (!isAlive()) return;
    
    // Kolor wg typu/stanu
    if (type == CHASER) {
        SDL_SetTextureColorMod(texture, 255, 100, 100); // Czerwony
    } else {
        SDL_SetTextureColorMod(texture, 100, 100, 255); // Niebieski
    }
    
    if (state == STUNNED) {
        SDL_SetTextureColorMod(texture, 255, 255, 100); // Żółty (stunned)
    }
    
    GameObject::draw(renderer, camera, false);
    // Reset koloru
    SDL_SetTextureColorMod(texture, 255, 255, 255);
}