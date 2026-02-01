#include "Camera.h"
#include "Tween.h"
#include "Dice.hpp"
#include "C.hpp"

Camera::Camera(const std::vector<Entity>& entities)
    : entities(entities), view({ 0.0f, 0.0f }, { C::GRID_SIZE * this->tileWidth, (C::GRID_SIZE * this->tileWidth) / ((float)C::RES_X / C::RES_Y) })
{}

void Camera::update(float dt) {    
    const Entity& player = this->entities[0];
    this->center = player.pos;

    if (player.direction != this->oldPlayerDirection) {
        if (player.direction & RIGHT) {
            Tween::tween(&this->offset.x, this->horizontalOffset, 0.2f, Tween::Function::EaseOutBack);
        }
        else if (player.direction & LEFT) {
            Tween::tween(&this->offset.x, -this->horizontalOffset, 0.2f, Tween::Function::EaseOutBack);
        }
    }

    this->oldPlayerDirection = player.direction;

    this->doShake(dt);
}

void Camera::draw(sf::RenderWindow& window) {
    this->view.setSize({ C::GRID_SIZE * this->tileWidth, (C::GRID_SIZE * this->tileWidth) / ((float)C::RES_X / C::RES_Y) });
    this->view.setCenter(this->center + this->offset);
    window.setView(this->view);
}

void Camera::doShake(float dt) {
    float strength = this->shakeTime > 0 ? this->shakeStrength : 0.0f;
    sf::Vector2f offset = { strength * Dice::randF2(), strength * Dice::randF2() };
    this->center += offset;

    shakeTime -= dt;
}
