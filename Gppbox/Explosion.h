#pragma once
#include "SFML/Graphics.hpp"
#include "Tween.h"

class Explosion {
public:
    Explosion(sf::Vector2f pos = { -1000.0f, -1000.0f });

    void start(sf::Vector2f pos);

    void update(float dt);
    void draw(sf::RenderWindow& window);

    sf::Vector2f pos;

    bool over = true;

    float r, g, b, a;
    float radius;
    float goalRadius = 64.0f;
    bool followPlayer = false;

    Tween::Tweener* tweener;

    sf::CircleShape shape;
};
