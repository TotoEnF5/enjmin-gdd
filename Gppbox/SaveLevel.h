#pragma once

#include "Entity.h"

void SaveLevel(const char* path, const std::vector<sf::Vector2i>& walls, const std::vector<Entity>& entities);
void OpenLevel(const char* path, std::vector<sf::Vector2i>& walls, std::vector<Entity>& entities);
