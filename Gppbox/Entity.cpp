#include "Entity.h"
#include "Game.hpp"
#include "C.hpp"
#include "imgui.h"
#include <iostream>
// #include <Windows.h>

std::vector<std::string> Entity::toDelete;

Entity::Entity(const std::string& name, Type type, sf::Vector2i tile, sf::Vector2f ratio)
    : NAME(name), TYPE(type), tile(tile), ratio(ratio)
{
    if (this->TYPE == Type::Projectile) {
        this->affectedByGravity = false;
        this->affectedByCollisions = false;
    }

    this->direction = RIGHT;

    switch (this->TYPE) {
    case Type::Player:
        this->size = C::GRID_SIZE;
        this->rectangleShape.setSize({ C::GRID_SIZE, C::GRID_SIZE });
        this->rectangleShape.setRadius(1.0f);
        this->rectangleShape.setFillColor(sf::Color::White);
        break;

    case Type::Enemy:
        this->size = C::GRID_SIZE;
        this->rectangleShape.setSize({ C::GRID_SIZE, C::GRID_SIZE });
        this->rectangleShape.setRadius(1.0f);
        this->rectangleShape.setFillColor(sf::Color::Red);
        break;

    case Type::Projectile:
        this->size = 8.0f;
        this->circleShape.setPointCount(6);
        this->circleShape.setRadius(8.0f);
        this->circleShape.setFillColor(sf::Color::Yellow);
        this->speed *= 3.0f;
        this->directionOffset.x = Dice::randF2();
        this->directionOffset.y = Dice::randF2() * 7;
        break;

    case Type::Pet:
        this->size = 4.0f;
        this->doSyncPos = false;
        this->circleShape.setRadius(4.0f);
        this->circleShape.setFillColor(sf::Color::White);
        break;
        
    case Type::HomingProjectile:
        this->size = 8.0f;
        this->doSyncPos = false;
        this->circleShape.setPointCount(3);
        this->circleShape.setRadius(8.0f);
        this->circleShape.setFillColor(sf::Color::Red);
        break;
    }

    this->initSize = this->size;

    this->syncPos();
}

void Entity::update(Game* game, const std::vector<sf::Vector2i>& walls, std::vector<Entity>& entities, float dt) {
    if (!this->doUpdate) {
        return;
    }

    switch (this->TYPE) {
    case Type::Player:
        break;

    case Type::Enemy:
        this->enemyUpdate(game, walls, entities);
        break;

    case Type::Projectile:
        this->projectileUpdate(game, walls, entities, dt);
        break;

    case Type::Pet:
        this->petUpdate(entities, dt);
        break;
        
    case Type::HomingProjectile:
        this->homingProjectileUpdate(game, entities, dt);
        break;
    }

    // apply jump + gravity
    if (this->affectedByGravity) {
        if (this->jump > 0) {
            this->delta.y -= this->jump;
        }
        if (!this->grounded) {
            this->jump -= this->gravity * dt;
        }
        this->delta.y += this->gravity;
    }

    if (this->affectedByCollisions) {
        this->collision(walls, dt);
    }

    // apply delta
    this->deltaLastFrame = this->delta;
    this->ratio += this->delta * dt;
    this->delta = { 0.0f, 0.0f };

    if (this->doSyncPos) {
        this->normalizePos();
        this->syncPos();
    }

    this->rectangleShape.setSize(sf::Vector2f(this->size, this->size));
    this->circleShape.setRadius(this->size);
}

void Entity::draw(sf::RenderWindow& window) {
    switch (this->TYPE) {
    case Type::Player:
        this->rectangleShape.setPosition(this->pos);
        window.draw(this->rectangleShape);
        break;

    case Type::Enemy:
        this->rectangleShape.setPosition(this->pos);
        window.draw(this->rectangleShape);
        break;

    case Type::Projectile:
        this->circleShape.setPosition(this->pos);
        window.draw(this->circleShape);
        break;

    case Type::Pet:
        this->circleShape.setPosition(this->pos);
        window.draw(this->circleShape);
        break;
        
    case Type::HomingProjectile:
        this->circleShape.setPosition(this->pos);
        window.draw(this->circleShape);
        break;
    }
}

void Entity::im() {
    if (ImGui::TreeNode(this->NAME.c_str())) {
        switch (this->TYPE) {
        case Type::Player:
            ImGui::Text("Player");
            break;
        case Type::Enemy:
            ImGui::Text("Enemy");
            break;
        case Type::Projectile:
            ImGui::Text("Projectile");
            break;
        default:
            ImGui::Text("%d", this->TYPE);
            break;
        }
        ImGui::DragInt2("Tile", (int*)&this->tile);
        ImGui::DragFloat2("Ratio", (float*)&this->ratio);
        ImGui::DragFloat2("Absolute position", (float*)&this->pos);
        ImGui::DragFloat2("Delta", (float*)&this->deltaLastFrame);
        ImGui::DragFloat("Speed", &this->speed);
        ImGui::DragFloat("Jump", &this->jumpForce);
        ImGui::DragFloat("Gravity", &this->gravity);
        ImGui::Checkbox("Affected by gravity", &this->affectedByGravity);
        ImGui::Checkbox("Update", &this->doUpdate);
        ImGui::Text("Grounded: %d", this->grounded);
        ImGui::Text("Direction mask: %d%d%d%d", (this->direction & RIGHT) >> 3, (this->direction & LEFT) >> 2, (this->direction & DOWN) >> 1, this->direction & UP);

        if (ImGui::Button("Delete entity")) {
            Entity::toDelete.push_back(this->NAME);
        }

        ImGui::TreePop();
    }
}

void Entity::doJump() {
    if (this->grounded) {
        this->jump = this->jumpForce;
    }
}

void Entity::shoot(std::vector<Entity>& entities) {
    int now = Game::clock.getElapsedTime().asMilliseconds();
    if (now - this->lastShoot > this->shootCooldown) {
        std::string name = "Projectile";
        name += std::to_string(entities.size());

        Entity& e = entities.emplace_back(name, Type::Projectile, this->tile, this->ratio);
        e.direction = this->direction;
        this->lastShoot = now;

        if (this->direction & RIGHT) {
            this->delta.x -= 3.0f;
        }
        if (this->direction & LEFT) {
            this->delta.x += 3.0f;
        }
    }
}

void Entity::shootHoming(std::vector<Entity>& entities) {
    int now = Game::clock.getElapsedTime().asMilliseconds();
    if (now - this->lastHoming > this->homingCooldown) {
        std::string name = "Homing";
        name += std::to_string(entities.size());

        Entity& e = entities.emplace_back(name, Type::HomingProjectile, this->tile, this->ratio);
        this->lastHoming = now;
    }
}

void Entity::squish() {
    Tween::tween(&this->size, this->size * 0.8f, 0.2f, Tween::Function::Linear);
}

void Entity::unsquish() {
    Tween::tween(&this->size, this->initSize, 0.2f, Tween::Function::Linear);
}

bool Entity::collides(sf::Vector2i other) {
    return Entity::collides(this->tile, other);
}

Entity Entity::operator=(const Entity& other) {
    this->NAME = other.NAME;
    this->TYPE = other.TYPE;
    this->tile = other.tile;
    this->ratio = other.ratio;
    this->pos = other.pos;
    this->delta = other.delta;
    this->deltaLastFrame = other.deltaLastFrame;
    this->jump = other.jump;
    this->grounded = other.grounded;
    this->affectedByGravity = other.affectedByGravity;
    this->affectedByCollisions = other.affectedByCollisions;
    this->doUpdate = other.doUpdate;
    this->speed = other.speed;
    this->gravity = other.gravity;
    this->jumpForce = other.jumpForce;
    this->shootCooldown = other.shootCooldown;
    this->lastShoot = other.lastShoot;
    this->direction = other.direction;
    return *this;
}

void Entity::enemyUpdate(Game* game, const std::vector<sf::Vector2i>& walls, std::vector<Entity>& entities) {
    for (Entity& entity : entities) {
        if (entity.NAME == this->NAME) {
            continue;
        }

        if (entity.TYPE == Type::Player && this->collides(entity.tile)) {
            entity.jump += entity.jumpForce / 3;
            if (this->direction & RIGHT) {
                entity.delta.x += entity.jumpForce;
            }
            if (this->direction & LEFT) {
                entity.delta.x -= entity.jumpForce;
            }
            Entity::toDelete.push_back(this->NAME);
            Game::spawnExplosion(this->pos);
            game->camera.shakeStrength = 10.0f;
            game->camera.shakeTime = 0.2f;
            Entity::toDelete.push_back(this->NAME);
        }
    }
    
    if (!this->grounded) {
        return;
    }
    
    for (const sf::Vector2i& wall : walls) {
        if (this->direction & RIGHT && Entity::collides({ this->tile.x + 1, this->tile.y }, wall)) {
            this->direction &= UP | DOWN | LEFT;
            this->direction |= LEFT;
        }

        if (this->direction & LEFT && Entity::collides({ this->tile.x - 1, this->tile.y }, wall)) {
            this->direction &= UP | DOWN | RIGHT;
            this->direction |= RIGHT;
        }
    }

    if (this->direction & RIGHT) {
        this->delta.x += this->speed;
    }
    else if (this->direction & LEFT) {
        this->delta.x -= this->speed;
    }
}

void Entity::projectileUpdate(Game* game, const std::vector<sf::Vector2i>& walls, const std::vector<Entity>& entities, float dt) {
    this->circleShape.setRotation(this->circleShape.getRotation() + dt * 1000);
    
    bool destroy = false;

    for (const Entity& entity : entities) {
        if (entity.NAME == this->NAME) {
            continue;
        }

        if (entity.TYPE == Type::Enemy && this->collides(entity.tile)) {
            Entity::toDelete.push_back(entity.NAME);
            Entity::toDelete.push_back(this->NAME);
            destroy = true;
        }
    }

    for (const sf::Vector2i& wall : walls) {
        if (this->tile == wall) {
            destroy = true;
            break;
        }

        if (this->direction & RIGHT) {
            if (Entity::collides({ this->tile.x + 1, this->tile.y }, wall)) {
                this->ratio.x = 0.0f;
                this->delta.x = 0.0f;
                destroy = true;
            }
        }

        if (this->direction & LEFT) {
            if (Entity::collides({ this->tile.x, this->tile.y }, wall)) {
                this->ratio.x = 1.0f;
                this->delta.x = 0.0f;
                destroy = true;
            }
        }
    }
    
    if (destroy) {
        // boom
        Game::spawnExplosion(this->pos);
        game->camera.shakeStrength = 10.0f;
        game->camera.shakeTime = 0.2f;
        Entity::toDelete.push_back(this->NAME);
    }

    if (this->direction & RIGHT) {
        this->delta.x += this->speed + this->directionOffset.x;
        this->delta.y += this->directionOffset.y;
    }
    else if (this->direction & LEFT) {
        this->delta.x -= this->speed + this->directionOffset.x;
        this->delta.y += this->directionOffset.y;
    }
}

void Entity::petUpdate(const std::vector<Entity>& entities, float dt) {
    this->pos.x = Tween::linear(this->pos.x, entities[0].pos.x, dt * 4);
    this->pos.y = Tween::linear(this->pos.y, entities[0].pos.y - 10, dt * 4);
}

void Entity::homingProjectileUpdate(Game* game, const std::vector<Entity>& entities, float dt) {
    sf::Vector2f target = this->pos;
    std::string name = ":";
    for (const Entity& entity : entities) {
        if (entity.TYPE == Type::Enemy) {
            target = entity.pos;
            name = entity.NAME;
            break;
        }
    }

    if ((this->pos.x >= target.x - 10 && this->pos.x <= target.x + 10) && (this->pos.y >= target.y - 10 && this->pos.y <= target.y + 10)) {
        Game::spawnExplosion(this->pos);
        game->camera.shakeStrength = 10.0f;
        game->camera.shakeTime = 0.2f;
        Entity::toDelete.push_back(this->NAME);
        Entity::toDelete.push_back(name);
    }
    
    this->pos.x = Tween::linear(this->pos.x, target.x, dt * 10);
    this->pos.y = Tween::linear(this->pos.y, target.y, dt * 10);
}

void Entity::collision(const std::vector<sf::Vector2i>& walls, float dt) {
    this->grounded = false;

    if (this->delta.y < 0.0f) {
        for (const sf::Vector2i& wall : walls) {
            bool collision = Entity::collides({ this->tile.x, this->tile.y - 1 }, wall) ||
                (this->ratio.x > 0.0f && Entity::collides({ this->tile.x + 1, this->tile.y - 1 }, wall));
            
            if (collision) {
                this->delta.y = 0.0f;
                this->ratio.y = 0.0f;
                this->jump = 0.0f;
                break;
            }
        }
    }
    
    if (this->delta.y > 0.0f) {
        for (const sf::Vector2i& wall : walls) {
            bool collision = Entity::collides({ this->tile.x, this->tile.y + 1 }, wall) ||
                (this->ratio.x > 0.0f && Entity::collides({ this->tile.x + 1, this->tile.y + 1 }, wall));
            
            if (collision) {
                this->delta.y = 0.0f;
                this->ratio.y = 0.0f;
                this->grounded = true;
                break;
            }
        }
    }
    
    if (this->delta.x > 0.0f) {
        for (const sf::Vector2i& wall : walls) {
            bool collision = Entity::collides({ this->tile.x + 1, this->tile.y }, wall) ||
                (this->ratio.y > 0.0f && Entity::collides({ this->tile.x + 1, this->tile.y + 1 }, wall));
            
            if (collision) {
                this->delta.x = 0.0f;
                this->ratio.x = 0.0f;
                break;
            }
        }
    }
    
    if (this->delta.x < 0.0f) {
        for (const sf::Vector2i& wall : walls) {
            bool collision = Entity::collides({ this->tile.x - 1, this->tile.y }, wall) ||
                (this->ratio.y > 0.0f && Entity::collides({ this->tile.x - 1, this->tile.y + 1 }, wall));
            
            if (collision) {
                this->delta.x = 0.0f;
                this->ratio.x = 0.0f;
                break;
            }
        }
    }
}

void Entity::normalizePos() {
    Entity::normalizeVectors(this->tile, this->ratio);
}

void Entity::syncPos() {
    this->pos.x = ((float)this->tile.x + this->ratio.x) * C::GRID_SIZE;
    this->pos.y = ((float)this->tile.y + this->ratio.y) * C::GRID_SIZE;
}

bool Entity::collides(sf::Vector2i a, sf::Vector2i b) {
    return a.x == b.x && a.y == b.y;
}

void Entity::normalizeVectors(sf::Vector2i& tile, sf::Vector2f& ratio) {
    while (ratio.x > 1.0f) {
        ratio.x--;
        tile.x++;
    }

    while (ratio.x < 0.0f) {
        ratio.x++;
        tile.x--;
    }

    while (ratio.y > 1.0f) {
        ratio.y--;
        tile.y++;
    }

    while (ratio.y < 0.0f) {
        ratio.y++;
        tile.y--;
    }
}
