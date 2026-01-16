#ifndef INPUT_BUFFER_H
#define INPUT_BUFFER_H

#include <SDL2/SDL.h>

enum InputKey {
    KEY_NONE = 0,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_ATTACK_LIGHT,
    KEY_ATTACK_HEAVY,
    KEY_JUMP
};

struct InputEvent {
    InputKey key;
    float timestamp;
    bool consumed;
};

class InputBuffer {
    private:
    bool keyPressed[(int)KEY_JUMP + 1];
    static const int MAX_BUFFER_SIZE = 10;
    InputEvent buffer[MAX_BUFFER_SIZE];
    int head;
    int size;
    float maxComboTime;

    bool checkDoubleTap(InputKey key, float currentTime, float maxTimeBetween = 0.3f);
    bool checkDirectionalDash(float currentTime, InputKey* outDirection);

    public:
    InputBuffer(float maxTime = 1.5f);

    void addInput(InputKey key, float currentTime);
    bool checkSequence(const InputKey* sequence, int length, float currentTime);
    bool isDashing(float currentTime, InputKey* outDirection = nullptr);
    void clearOldInputs(float currentTime);
    void clear();
    void debugPrint(float currentTime);
    void addKeyPress(InputKey key, float currentTime);
    void keyReleased(InputKey key);
    
    static InputKey scancodeToInput(SDL_Scancode scancode);
};

#endif