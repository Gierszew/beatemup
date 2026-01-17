#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "SDL2/SDL.h"
#include "camera.h"
#include "hitbox.h"

class GameObject {
    protected:
    Hitbox bodyHitbox;
    Hitbox attackHitbox;

    public:
    GameObject();

    void load(float x, float y, int width, int height);
    void load(float x, float y, int width, int height, char* textureId);
    void setTexture(SDL_Renderer* renderer);
    void draw(SDL_Renderer* renderer, Camera* camera, bool flip = false);
    void drawWithoutTexture(SDL_Renderer* renderer, Camera* camera);
    void update();
    void clean();
    void drawDebug(SDL_Renderer* renderer, Camera* camera);

    virtual Hitbox getBodyHitbox() const {
        return bodyHitbox;
    }

    virtual Hitbox getAttackHitbox() const {
        return attackHitbox;
    }

    void setBodyHitbox(float offsetX, float offsetY, float w, float h) {
        bodyHitbox = Hitbox(offsetX, offsetY, w, h);
        printf("DEBUG: setBodyHitbox offset=(%.0f,%.0f) size=(%.0fx%.0f) for object at (%.0f,%.0f)\n",
           offsetX, offsetY, w, h, x, y);
    }

    void setAttackHitbox(float offsetX, float offsetY, float w, float h) {
        attackHitbox = Hitbox(offsetX, offsetY, w, h);
    }

    bool collidesWith(const GameObject* other) const {
        return bodyHitbox.collidesWith(x, y, other->getBodyHitbox(), other->x, other->y);
    }

    void setPosition(float newX, float newY) {
        x = newX;
        y = newY;
    }

    void move(float dx, float dy) {
        x += dx;
        y += dy;
    }

    SDL_Texture* texture;

    char* textureId;

    int originalWidth;
    int originalHeight;
    int currentFrame;
    int currentRow;
    int worldWidth;
    int worldHeight;
    float x;
    float y;
    int width;
    int height;

    float speed;
    float scale;

    bool movingLeft;
    bool alreadyDamaged;
};

#endif