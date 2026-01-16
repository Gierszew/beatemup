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
    
    // 1. Sprawdź czy to nie jest to samo wciśnięcie co ostatnie
    if (size > 0) {
        int lastIdx = head;
        if (buffer[lastIdx].key == key) {
            // To samo wciśnięcie - sprawdź czas
            float timeSinceLast = currentTime - buffer[lastIdx].timestamp;
            if (timeSinceLast < 0.001f) {  // 50ms debounce
                printf("[InputBuffer] Debouncing: same key pressed too soon (%.3fs)\n", 
                       timeSinceLast);
                return;  // Ignoruj zbyt szybkie powtórzenia
            }
        }
    }
    
    // 2. Dodaj nowe wejście
    head = (head + 1) % MAX_BUFFER_SIZE;
    buffer[head] = {key, currentTime, false};
    
    if (size < MAX_BUFFER_SIZE) {
        size++;
    } else {
        // Bufer pełny - nadpisz najstarszy (circular buffer)
        printf("[InputBuffer] Buffer full, overwriting oldest\n");
    }
    
    printf("[InputBuffer] Added %d (size: %d/%d)\n", key, size, MAX_BUFFER_SIZE);
}

bool InputBuffer::checkSequence(const InputKey* sequence, int length, float currentTime) {
    if (length == 0 || size < length) return false;
    
    // Sprawdź od najnowszego do najstarszego
    int seqIndex = length - 1;
    
    for (int i = 0; i < size && seqIndex >= 0; i++) {
        int idx = (head - i + MAX_BUFFER_SIZE) % MAX_BUFFER_SIZE;
        
        if (buffer[idx].consumed) continue;
        
        // Sprawdź czy wejście nie jest za stare
        if (currentTime - buffer[idx].timestamp > maxComboTime) {
            continue;
        }
        
        // Sprawdź czy pasuje do sekwencji
        if (buffer[idx].key == sequence[seqIndex]) {
            seqIndex--;
        } else {
            // Sekwencja przerwana - zaczynamy od nowa
            seqIndex = length - 1;
        }
    }
    
    if (seqIndex < 0) {
        // Znaleziono sekwencję! Oznacz użyte wejścia
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
            buffer[idx].consumed = true;  // Oznacz jako "wygasłe"
        }
    }
}

void InputBuffer::clear() {
    size = 0;
    head = 0;
    memset(buffer, 0, sizeof(buffer));
}

void InputBuffer::debugPrint(float currentTime) {
    printf("InputBuffer [%d/%d]: ", size, MAX_BUFFER_SIZE);
    
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
        
        printf("%s(%.1fs%s) ", keyName, 
               currentTime - buffer[idx].timestamp,
               buffer[idx].consumed ? "*" : "");
        
        if (i < size - 1) printf("<- ");
    }
    printf("\n");
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
            // Oznacz jako użyte
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
    if (checkDoubleTap(KEY_LEFT, currentTime, 0.3f)) {
        if (outDirection) *outDirection = KEY_LEFT;
        return true;
    }
    
    // Sprawdź dash w prawo (→→)
    if (checkDoubleTap(KEY_RIGHT, currentTime, 0.3f)) {
        if (outDirection) *outDirection = KEY_RIGHT;
        return true;
    }
    
    // Sprawdź dash w górę/dół (opcjonalnie)
    if (checkDoubleTap(KEY_UP, currentTime, 0.3f)) {
        if (outDirection) *outDirection = KEY_UP;
        return true;
    }
    
    if (checkDoubleTap(KEY_DOWN, currentTime, 0.3f)) {
        if (outDirection) *outDirection = KEY_DOWN;
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
    
    // Jeśli klawisz już wciśnięty - ignoruj
    if (keyPressed[keyIndex]) {
        return;
    }
    
    // Oznacz jako wciśnięty i dodaj do bufora
    keyPressed[keyIndex] = true;
    addInput(key, currentTime);
    
    printf("[KeyPress] %d registered\n", key);
}

void InputBuffer::keyReleased(InputKey key) {
    if (key == KEY_NONE) return;
    keyPressed[(int)key] = false;
}