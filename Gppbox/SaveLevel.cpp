#include "SaveLevel.h"
#include <iostream>
#include <fstream>

void SaveLevel(const char* path, const std::vector<sf::Vector2i>& walls, const std::vector<Entity>& entities) {
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cout << "Failed to open " << path << std::endl;
        return;
    }

    file << "COOLLEVEL" << std::endl;

    file << walls.size() << std::endl;
    for (const sf::Vector2i& wall : walls) {
        file << wall.x << " " << wall.y << std::endl;
    }

    file << entities.size() << std::endl;
    for (const Entity& entity : entities) {
        file << entity.NAME << " " << (int)entity.TYPE << " " << entity.tile.x << " " << entity.tile.y << " " << entity.direction << std::endl;
    }

    file.close();
};

void OpenLevel(const char* path, std::vector<sf::Vector2i>& walls, std::vector<Entity>& entities) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "Failed to open " << path << std::endl;
        return;
    }
    
    std::string magic;
    file >> magic;
    if (magic != "COOLLEVEL") {
        std::cout << "Not a clf5 file!" << std::endl;
        return;
    }

    walls.clear();

    int numWalls;
    file >> numWalls;
    for (int i = 0; i < numWalls; i++) {
        sf::Vector2i wall;
        file >> wall.x;
        file >> wall.y;
        walls.push_back(wall);
    }

    entities.clear();

    int numEntities;
    file >> numEntities;
    for (int i = 0; i < numEntities; i++) {
        std::string name;
        int type;
        file >> name;
        file >> type;

        Entity entity(name, (Entity::Type)type);
        file >> entity.tile.x;
        file >> entity.tile.y;
        file >> entity.direction;

        entities.push_back(entity);
    }

    file.close();
}
