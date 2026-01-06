#include "player.h"

void Player::load(int x, int y, int width, int height, char* textureId) {
    GameObject::load(x, y, width, height, textureId);
}

void Player::setTexture(SDL_Renderer* renderer) {
    GameObject::setTexture(renderer);
}

void Player::draw(SDL_Renderer* renderer) {
    GameObject::draw(renderer);
}

void Player::update() {
    // Implement player-specific update logic here
}