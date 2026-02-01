#include "Editor.h"
#include "Entity.h"
#include "SaveLevel.h"
#include "C.hpp"
#include "imgui.h"

Editor::Editor(std::vector<sf::Vector2i>& walls, std::vector<Entity>& entities)
    : walls(walls), entities(entities)
{
    this->shape.setSize({ C::GRID_SIZE, C::GRID_SIZE });
    this->shape.setFillColor(sf::Color(255, 255, 255, 128));
}

void Editor::draw(sf::RenderWindow& window) {
    if (this->enabled) {
        window.draw(this->shape);
    }
}

void Editor::im() {
    ImVec2 mousePos = ImGui::GetMousePos();
    sf::Vector2i tilePos = { (int)mousePos.x / C::GRID_SIZE, (int)mousePos.y / C::GRID_SIZE };
    this->shape.setPosition({ (float)tilePos.x * 16, (float)tilePos.y * 16 });

    static char name[128] = "Unnamed";
    Entity::Type type;
    if (ImGui::CollapsingHeader("Editor")) {
        if (ImGui::Checkbox("Enabled", &this->enabled)) {
            this->toggleEnable();
        }

        ImGui::Checkbox("Update new entities", &this->updateSpawnedEntities);

        if (ImGui::Button("Update all entities")) {
            for (Entity& entity : this->entities) {
                entity.doUpdate = true;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop updating all entities")) {
            for (Entity& entity : this->entities) {
                entity.doUpdate = false;
            }
        }

        Mode modes[] = { Mode::Wall, Mode::Entity, Mode::Remove };
        const char* modesStr = "Wall\0Entity\0Remove\0";
        static int modeIndex = 0;
        ImGui::Combo("Mode", &modeIndex, modesStr);
        this->mode = modes[modeIndex];

        if (this->mode == Mode::Entity) {
            ImGui::InputText("Name", name, 128);

            ImGui::SameLine();

            Entity::Type types[] = { Entity::Type::Enemy, Entity::Type::Projectile };
            const char* typesStr = "Enemy\0Projectile\0";
            static int typeIndex = 0;
            ImGui::Combo("Type", &typeIndex, typesStr);
            type = types[typeIndex];
        }

        ImGui::Separator();

        static char levelName[128] = "Unnamed level.clf5"; // .clf5 pcq cl c pour "cool level" et f5 c dans mon pseudo
        ImGui::InputText("File name", levelName, 128);
        ImGui::SameLine();
        if (ImGui::Button("Save level")) {
            SaveLevel(levelName, this->walls, this->entities);
        }
        ImGui::SameLine();
        if (ImGui::Button("Open level")) {
            OpenLevel(levelName, this->walls, this->entities);
        }

        ImGui::Text("Mouse tile: %d, %d", tilePos.x, tilePos.y);
    }

    if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && !ImGui::IsAnyItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        switch (this->mode) {
        case Mode::Wall: {
            bool exists = false;
            for (const sf::Vector2i& wall : this->walls) {
                if (tilePos.x == wall.x && tilePos.y == wall.y) {
                    exists = true;
                    break;
                }
            }

            if (!exists) {
                this->walls.push_back(tilePos);
            }
            break;
        }

        case Mode::Remove: {
            int i = 0;
            int index = -1;
            for (const sf::Vector2i& wall : this->walls) {
                if (wall.x == tilePos.x && wall.y == tilePos.y) {
                    index = i;
                }

                i++;
            }

            if (index > 0) {
                this->walls.erase(this->walls.begin() + index);
            }

            for (const Entity& entity : entities) {
                if (entity.tile == tilePos) {
                    Entity::toDelete.push_back(entity.NAME);
                }
            }
            break;
        }

        default:
            break;
        }
    }

    if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered() && this->mode == Mode::Entity) {
        std::string realName(name);
        realName += std::to_string(this->entities.size());
        Entity& entity = this->entities.emplace_back(realName.c_str(), type, tilePos);
        entity.doUpdate = this->updateSpawnedEntities;
    }
}

void Editor::toggleEnable() {
    if (this->enabled) {
        this->shape.setFillColor(sf::Color(255, 255, 255, 128));
    }
    else {
        this->shape.setFillColor(sf::Color::Transparent);
    }
}
