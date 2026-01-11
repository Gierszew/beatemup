#ifndef ENEMY_H
#define ENEMY_H

#include "gameobject.h"
#include "player.h"

enum EnemyType {
    CHASER,
    RANGED
};

enum EnemyState {
    PATROLLING,
    CHASING,
    ATTACKING,
    STUNNED,
    DEAD
};

class Enemy : public GameObject {
    private:
    EnemyState state;
    EnemyType type;
    Player* target;

    int health;
    int damage;

    float attackRange;
    float attackCooldown;
    float stunTimer;
    float scale;

    void updateChaser(float dt);
    void updateRanged(float dt);

    const float CHASER_SPEED = 200.0f;
    const float RANGED_SPEED = 150.0f;
    const float CHARGE_SPEED = 400.0f;
    const float ATTACK_DISTANCE = 100.0f;
    const float STUN_TIME = 0.5f;
    const int MAX_HEALTH = 3;

    public:
    Enemy();
    void init(EnemyType type, float x, float y, Player* target);
    void update(float dt);
    void draw(SDL_Renderer* renderer, Camera* camera);

    void takeDamage(int damage);
    bool isAlive() {return health > 0;}
    bool isStunned() {return state == STUNNED;}
    bool isAttacking() {return state == ATTACKING;}

    EnemyType getType() const { return type; }
    EnemyState getState() const { return state; }
    int getHealth() const { return health; }
    int getDamage() const { return damage; }
};

#endif