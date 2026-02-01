#include "Explosion.h"

#include "Dice.hpp"
#include "Tween.h"

Explosion::Explosion(sf::Vector2f pos) {
    this->pos = pos;
    this->over = true;
    this->shape.setPointCount(8);
}

void Explosion::start(sf::Vector2f pos) {
    this->over = false;

    this->pos = pos;
    this->shape.setPosition(this->pos);

    this->shape.setRotation(Dice::angleDeg());

    this->radius = 0.0f;
    this->r = 1.0f;
    this->g = 1.0f;
    this->b = 0.0f;
    this->a = 1.0f;

    Tween::tween(&this->radius, this->goalRadius, 0.2f, Tween::Function::EaseOutExpo);
    Tween::tween(&this->r, 0.1f, 0.2f, Tween::Function::Linear);
    Tween::tween(&this->g, 0.1f, 0.2f, Tween::Function::Linear);
    this->tweener = Tween::tween(&this->a, 0.0f, 0.2f, Tween::Function::Linear);
}

void Explosion::update(float dt) {
    this->shape.setOrigin({ this->radius / 2.0f, this->radius / 2.0f });
    this->shape.setRadius(this->radius);
    this->shape.setFillColor(sf::Color(this->r * 255, this->g * 255, this->b * 255, this->a * 255));

    this->over = this->tweener->isOver;
    if (this->over) {
        this->shape.setPosition({ -1000.0f, -1000.0f });
    }
}

void Explosion::draw(sf::RenderWindow& window) {
    window.draw(shape);
}
