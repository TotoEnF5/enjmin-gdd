#pragma once
#include "Entity.h"

class Camera {
public:
    Camera(const std::vector<Entity>& entities);

    void update(float dt);
    void draw(sf::RenderWindow& window);

    float horizontalOffset = 64.0f;
    sf::Vector2f offset = { 0.0f, 0.0f };

    float shakeStrength = 0.0f;
    float shakeTime = 0.0f;

    float tileWidth = 40.0f;

    char oldPlayerDirection = 0;

    const std::vector<Entity>& entities;
    sf::Vector2f center;
    sf::View view;

private:
    void doShake(float dt);
};
