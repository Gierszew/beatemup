#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include "gameobject.h"
#include "camera.h"

enum PlayerState {
    IDLE,
    WALKING,
    ATTACKING_LIGHT,
    ATTACKING_HEAVY,
    HURT
};

class Player : public GameObject {
    private:
    float attackTimer;
    float attackCooldown;
    const float LIGHT_ATTACK_TIME = 0.2f;
    const float HEAVY_ATTACK_TIME = 0.5f;
    const float ATTACK_COOLDOWN = 0.3f;

    public:
    Player();

    void load(int x, int y, int width, int height, char* textureId);
    void setTexture(SDL_Renderer* renderer);
    void draw(SDL_Renderer* renderer, Camera* camera);
    void update(float dt);
    void clean();
    void handleInput(float dt);

    void attackLight();
    void attackHeavy();
    void takeDamage(int damage);

    bool isAttacking() {
        return state == ATTACKING_LIGHT || state == ATTACKING_HEAVY;
    }

    PlayerState state;
    int hp;

};

#endif