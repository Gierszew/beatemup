#ifndef HITBOX_H
#define HITBOX_H

#include <SDL2/SDL.h>

struct Hitbox {
    float x, y;
    float width, height;

    Hitbox() : x(0), y(0), width(0), height(0) {}

    Hitbox(float x, float y, float width, float height)
        : x(x), y(y), width(width), height(height) {}
    
    SDL_Rect getRect(float objX, float objY) const {
        return {
            (int)(objX + x),
            (int)(objY + y),
            (int)width,
            (int)height
        };
    }

    bool collidesWith(float objX, float objY, const Hitbox& other, float otherX, float otherY) const {
        SDL_Rect rect1 = getRect(objX, objY);
        SDL_Rect rect2 = other.getRect(otherX, otherY);
        
        return (rect1.x < rect2.x + rect2.w &&
                rect1.x + rect1.w > rect2.x &&
                rect1.y < rect2.y + rect2.h &&
                rect1.y + rect1.h > rect2.y);
    }
};

#endif