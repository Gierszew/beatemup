#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include "gameobject.h"
#include "camera.h"
#include "inputbuffer.h"

enum PlayerState {
    IDLE,
    WALKING,
    ATTACKING_LIGHT,
    ATTACKING_HEAVY,
    HURT,
    DASHING
};

enum ComboType {
    COMBO_NONE = -1,
    COMBO_TRIPLE_LIGHT,      // L-L-L
    COMBO_TRIPLE_HEAVY,      // H-H-H  
    COMBO_MIXED,             // L-H-L
    COMBO_DASH_LEFT,         // ←←
    COMBO_DASH_RIGHT,        // →→
    COMBO_UPPERCUT,          // ↓→ + L
    MAX_COMBOS
};

struct ComboDefinition {
    ComboType type;
    const char* name;
    InputKey sequence[8];
    int length;
    float damageMultiplier;
};

class Player : public GameObject {
    private:
    float attackTimer;
    float attackCooldown;
    const float LIGHT_ATTACK_TIME = 0.2f;
    const float HEAVY_ATTACK_TIME = 0.5f;
    const float ATTACK_COOLDOWN = 0.3f;

    InputBuffer inputBuffer;
    ComboType currentCombo;
    float comboDamageMultiplier;
    float comboMultiplierTimer;
    float comboCooldown;

    float dashSpeed;
    float normalSpeed;
    float dashTimer;
    float dashCooldown;
    float dashOriginalSpeed;
    float invincibilityFrames;
    int dashDirection;
    const int MAX_HP = 100;
    
    bool showDebugInfo;
    static const ComboDefinition comboDefinitions[MAX_COMBOS];

    void executeCombo(ComboType combo);

    public:
    Player();

    void load(int x, int y, int width, int height, char* textureId);
    void setTexture(SDL_Renderer* renderer);
    void draw(SDL_Renderer* renderer, Camera* camera, bool debug);
    void update(float dt);
    void clean();
    void handleInput(float dt);

    void attackLight();
    void attackHeavy();
    void takeDamage(int damage);
    int getAttackDamage() const;

    void handleMovement(const Uint8* keystate, float currentTime);
    void checkCombos(float currentTime);
    
    void updateCombo(float dt);

    void performDash(bool left);
    void performUppercut();

    bool isAttacking() {
        return state == ATTACKING_LIGHT || state == ATTACKING_HEAVY;
    }

    float getAttackTimer() const {
        return attackTimer;
    }

    ComboType getCurrentCombo() const {
        return currentCombo;
    }
    const char* getComboName(ComboType combo) const;

    void toggleDebugInfo() {
        showDebugInfo = !showDebugInfo;
    }

    float getComboMultiplier() const { 
        return comboDamageMultiplier; 
    }

    InputBuffer* getInputBuffer() { return &inputBuffer; }

    PlayerState state;
    int hp;
    int score;

};

#endif