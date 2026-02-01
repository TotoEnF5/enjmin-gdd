#pragma once

#include <vector>
#include <array>

#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"
#include "SFML/Window.hpp"

#include "sys.hpp"

#include "Entity.h"
#include "Camera.h"
#include "Explosion.h"
#include "Editor.h"
#include "Particle.hpp"
#include "ParticleMan.hpp"

using namespace sf;

class HotReloadShader;
class Game {
public:
	sf::RenderWindow*				win = nullptr;

	sf::RectangleShape				bg;
	HotReloadShader *				bgShader = nullptr;

	sf::Texture						tex;

	bool							closing = false;
	
	std::vector<sf::Vector2i>		walls;
	std::vector<sf::RectangleShape> wallSprites;
	bool showimgui = false;

    std::vector<Entity> entities;
    static std::array<Explosion, 256> explosions;
    Camera camera;

	ParticleMan beforeParts;
	ParticleMan afterParts;

    Editor editor;

    static sf::Clock clock;

	Game(sf::RenderWindow * win);

	void cacheWalls();

	void processInput(sf::Event ev);
	bool wasPressed = false;
	void pollInput(double dt);
	void onSpacePressed();

	void update(double dt);

	void draw(sf::RenderWindow& win);

	bool isWall(int cx, int cy);
	void im();

    static void spawnExplosion(sf::Vector2f pos, float radius = 64.0f, bool followPlayer = false);
};
