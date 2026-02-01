#pragma once
#include <SFML/Graphics.hpp>
#include "RoundedRectangle.hpp"

#define UP 0b0001
#define DOWN 0b0010
#define LEFT 0b0100
#define RIGHT 0b1000

class Game;

class Entity {
public:
    enum class Type {
        Player,
        Enemy,
        Projectile,
        Pet,
        HomingProjectile,
    };

    Entity(const std::string& name, Type type, sf::Vector2i tile = { 0, 0 }, sf::Vector2f ratio = { 0.0f, 0.0f });

    void update(Game* game, const std::vector<sf::Vector2i>& walls, std::vector<Entity>& entities, float dt);
    void draw(sf::RenderWindow& window);
    void im();

    void doJump();
    void shoot(std::vector<Entity>& entities);
    void shootHoming(std::vector<Entity>& entities);

    void squish();
    void unsquish();

    bool collides(sf::Vector2i other);

    Entity operator=(const Entity& other);

    std::string NAME;
    Type TYPE;

    // jsuis pas désolé pour cette horreur
    float size;
    float initSize;
    RoundedRectangle rectangleShape;
    sf::CircleShape circleShape;

    sf::Vector2i tile;
    sf::Vector2f ratio;
    sf::Vector2f pos;
    sf::Vector2f delta;
    sf::Vector2f deltaLastFrame;

    float jump = 0.0f;
    bool grounded = false;
    bool affectedByGravity = true;
    bool affectedByCollisions = true;
    bool doUpdate = true;

    float speed = 30.0f;
    float gravity = 20.0f;
    float jumpForce = 33.0f;

    int shootCooldown = 100;
    int homingCooldown = 1000;
    int lastShoot = 0;
    int lastHoming = 0;

    char direction = 0;
    sf::Vector2f directionOffset;

    bool doSyncPos = true;

    static std::vector<std::string> toDelete;

private:
    void enemyUpdate(Game* game, const std::vector<sf::Vector2i>& walls, std::vector<Entity>& entities);
    void projectileUpdate(Game* game, const std::vector<sf::Vector2i>& walls, const std::vector<Entity>& entities, float dt);
    void petUpdate(const std::vector<Entity>& entities, float dt);
    void homingProjectileUpdate(Game* game, const std::vector<Entity>& entities, float dt);

    void collision(const std::vector<sf::Vector2i>& walls, float dt);
    void normalizePos();
    void syncPos();

    static bool collides(sf::Vector2i a, sf::Vector2i b);
    static void normalizeVectors(sf::Vector2i& tile, sf::Vector2f& ratio);
};
