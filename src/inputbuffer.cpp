#include "inputbuffer.h"
#include <stdio.h>
#include <string.h>

InputBuffer::InputBuffer(float maxTime) {
    head = 0;
    size = 0;
    maxComboTime = maxTime;
    memset(buffer, 0, sizeof(buffer));
    memset(keyPressed, 0, sizeof(keyPressed));
}

void InputBuffer::addInput(InputKey key, float currentTime) {
    if (key == KEY_NONE) return;
    
    if (size > 0) {
        int lastIdx = head;
        if (buffer[lastIdx].key == key) {
            float timeSinceLast = currentTime - buffer[lastIdx].timestamp;
            if (timeSinceLast < 0.001f) {
                return;
            }
        }
    }
    
    head = (head + 1) % MAX_BUFFER_SIZE;
    buffer[head] = {key, currentTime, false};
    
    if (size < MAX_BUFFER_SIZE) {
        size++;
    }
}

bool InputBuffer::checkSequence(const InputKey* sequence, int length, float currentTime) {
    if (length == 0 || size < length) return false;
    int seqIndex = length - 1;
    
    for (int i = 0; i < size && seqIndex >= 0; i++) {
        int idx = (head - i + MAX_BUFFER_SIZE) % MAX_BUFFER_SIZE;
        
        if (buffer[idx].consumed) continue;
        
        if (currentTime - buffer[idx].timestamp > maxComboTime) {
            continue;
        }
        
        if (buffer[idx].key == sequence[seqIndex]) {
            seqIndex--;
        } else {
            seqIndex = length - 1;
        }
    }
    
    if (seqIndex < 0) {
        int found = 0;
        for (int i = 0; i < size && found < length; i++) {
            int idx = (head - i + MAX_BUFFER_SIZE) % MAX_BUFFER_SIZE;
            if (!buffer[idx].consumed && buffer[idx].key == sequence[length - found - 1]) {
                buffer[idx].consumed = true;
                found++;
            }
        }
        return true;
    }
    
    return false;
}

void InputBuffer::clearOldInputs(float currentTime) {
    for (int i = 0; i < size; i++) {
        int idx = (head - i + MAX_BUFFER_SIZE) % MAX_BUFFER_SIZE;
        if (currentTime - buffer[idx].timestamp > maxComboTime) {
            buffer[idx].consumed = true;
        }
    }
}

void InputBuffer::clear() {
    size = 0;
    head = 0;
    memset(buffer, 0, sizeof(buffer));
}

void InputBuffer::debugPrint(float currentTime) {
    for (int i = 0; i < size; i++) {
        int idx = (head - i + MAX_BUFFER_SIZE) % MAX_BUFFER_SIZE;
        const char* keyName = "?";
        
        switch(buffer[idx].key) {
            case KEY_UP: keyName = "UP"; break;
            case KEY_DOWN: keyName = "DOWN"; break;
            case KEY_LEFT: keyName = "LEFT"; break;
            case KEY_RIGHT: keyName = "RIGHT"; break;
            case KEY_ATTACK_LIGHT: keyName = "ATK_L"; break;
            case KEY_ATTACK_HEAVY: keyName = "ATK_H"; break;
            case KEY_JUMP: keyName = "JUMP"; break;
            default: keyName = "NONE"; break;
        }
        
    }
}

const char* InputBuffer::keyToString(InputKey key) {
    switch(key) {
        case KEY_UP: return "UP";
        case KEY_DOWN: return "DOWN";
        case KEY_LEFT: return "LEFT";
        case KEY_RIGHT: return "RIGHT";
        case KEY_ATTACK_LIGHT: return "LIGHT";
        case KEY_ATTACK_HEAVY: return "HEAVY";
        default: return "UNKNOWN";
    }
}

InputKey InputBuffer::scancodeToInput(SDL_Scancode scancode) {
    switch(scancode) {
        case SDL_SCANCODE_W:
        case SDL_SCANCODE_UP:    return KEY_UP;
        case SDL_SCANCODE_S:
        case SDL_SCANCODE_DOWN:  return KEY_DOWN;
        case SDL_SCANCODE_A:
        case SDL_SCANCODE_LEFT:  return KEY_LEFT;
        case SDL_SCANCODE_D:
        case SDL_SCANCODE_RIGHT: return KEY_RIGHT;
        case SDL_SCANCODE_J:     return KEY_ATTACK_LIGHT;
        case SDL_SCANCODE_K:     return KEY_ATTACK_HEAVY;
        case SDL_SCANCODE_SPACE: return KEY_JUMP;
        default: return KEY_NONE;
    }
}

bool InputBuffer::checkDoubleTap(InputKey key, float currentTime, float maxTimeBetween) {
    if (size < 2) return false;

    InputEvent first = {KEY_NONE, 0, false};
    InputEvent second = {KEY_NONE, 0, false};

    for (int i = 0; i < size; i++) {
        int idx = (head - i + MAX_BUFFER_SIZE) % MAX_BUFFER_SIZE;
        
        if (!buffer[idx].consumed && buffer[idx].key == key) {
            if (second.key == KEY_NONE) {
                second = buffer[idx];
            } else {
                first = buffer[idx];
                break;
            }
        }
    }

    if (first.key == key && second.key == key) {
        float timeBetween = second.timestamp - first.timestamp;
        
        if (timeBetween > 0 && timeBetween <= maxTimeBetween) {
            for (int i = 0; i < size; i++) {
                int idx = (head - i + MAX_BUFFER_SIZE) % MAX_BUFFER_SIZE;
                if (buffer[idx].key == key && 
                    (buffer[idx].timestamp == first.timestamp || 
                     buffer[idx].timestamp == second.timestamp)) {
                    buffer[idx].consumed = true;
                }
            }
            return true;
        }
    }

    return false;
}

bool InputBuffer::checkDirectionalDash(float currentTime, InputKey* outDirection) {
    // Sprawdź dash w lewo (←←)
    if (checkDoubleTap(KEY_LEFT, currentTime, 0.1f)) {
        if (outDirection) *outDirection = KEY_LEFT;
        return true;
    }
    
    // Sprawdź dash w prawo (→→)
    if (checkDoubleTap(KEY_RIGHT, currentTime, 0.1f)) {
        if (outDirection) *outDirection = KEY_RIGHT;
        return true;
    }
    
    return false;
}

bool InputBuffer::isDashing(float currentTime, InputKey* outDirection) {
    return checkDirectionalDash(currentTime, outDirection);
}

void InputBuffer::addKeyPress(InputKey key, float currentTime) {
    if (key == KEY_NONE) return;
    
    int keyIndex = (int)key;
    
    if (keyPressed[keyIndex]) {
        return;
    }

    keyPressed[keyIndex] = true;
    addInput(key, currentTime);
}

void InputBuffer::keyReleased(InputKey key) {
    if (key == KEY_NONE) return;
    keyPressed[(int)key] = false;
}