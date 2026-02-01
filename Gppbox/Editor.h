#pragma once

#include <SFML/Graphics.hpp>
#include "Entity.h"

class Editor {
public:
    enum class Mode {
        Wall,
        Remove,
        Entity,
    };

    Editor(std::vector<sf::Vector2i>& walls, std::vector<Entity>& entities);

    void draw(sf::RenderWindow& window);
    void im();

    void toggleEnable();

    bool enabled = false;
    Mode mode;
    bool updateSpawnedEntities = false;
    std::vector<sf::Vector2i>& walls;
    std::vector<Entity>& entities;
    sf::RectangleShape shape;
};
