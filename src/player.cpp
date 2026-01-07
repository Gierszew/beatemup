#include "player.h"

void Player::load(int x, int y, int width, int height, char* textureId) {
    this->speed = 500.0f;
    GameObject::load(x, y, width, height, textureId);
}

void Player::setTexture(SDL_Renderer* renderer) {
    GameObject::setTexture(renderer);
}

void Player::draw(SDL_Renderer* renderer, Camera* camera) {
    GameObject::draw(renderer, camera->getViewPort());
}

void Player::update(float dt) {
    this->handleInput(dt);
}

void Player::handleInput(float dt) {
    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    if (keystate[SDL_SCANCODE_W]) y -= speed * dt;
    if (keystate[SDL_SCANCODE_S]) y += speed * dt;
    if (keystate[SDL_SCANCODE_A]) x -= speed * dt;
    if (keystate[SDL_SCANCODE_D]) x += speed * dt;
}
