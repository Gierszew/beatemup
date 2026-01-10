// worldbounds.h
#ifndef WORLDBOUNDS_H
#define WORLDBOUNDS_H

struct WorldBounds {
    int left, right, top, bottom;
    
    WorldBounds() : left(0), top(0), right(800), bottom(600) {}

    WorldBounds(int l, int t, int r, int b) : 
        left(l), top(t), right(r), bottom(b) {}
    
    bool isPointInside(float px, float py) {
        return px >= left && px <= right && py >= top && py <= bottom;
    }
    
    void clampPlayer(float* x, float* y, int width, int height) {
        // Lewa krawędź
        if (*x < left) *x = left;
        // Prawa krawędź
        if (*x + width > right) *x = right - width;
        // Góra
        if (*y < top) *y = top;
        // Dół
        if (*y + height > bottom) *y = bottom - height;
    }
};

#endif