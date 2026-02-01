
#include <imgui.h>
#include <array>
#include <vector>

#include "C.hpp"
#include "Game.hpp"
#include "Tween.h"

#include "HotReloadShader.hpp"
#include "SaveLevel.h"

#define PLAYER this->entities[0]

static int cols = C::RES_X / C::GRID_SIZE;
static int lastLine = C::RES_Y / C::GRID_SIZE - 1;

sf::Clock Game::clock;
std::array<Explosion, 256> Game::explosions;

Game::Game(sf::RenderWindow * win)
    : camera(this->entities), editor(this->walls, this->entities)
{
	this->win = win;
	bg = sf::RectangleShape(Vector2f((float)win->getSize().x, (float)win->getSize().y));

	bool isOk = tex.loadFromFile("res/bg_stars.png");
	if (!isOk) {
		printf("ERR : LOAD FAILED\n");
	}
	bg.setTexture(&tex);
	bg.setSize(sf::Vector2f(C::RES_X, C::RES_Y));

	bgShader = new HotReloadShader("res/bg.vert", "res/bg.frag");

	OpenLevel("level.clf5", walls, entities);
	cacheWalls();

    this->entities.emplace_back("Player", Entity::Type::Player, sf::Vector2i(6, 28));
	this->entities.emplace_back("pet", Entity::Type::Pet, sf::Vector2i(10, 54));
}

void Game::cacheWalls()
{
    std::vector<Vector2i> processedWalls;

	wallSprites.clear();
	for (Vector2i & w : walls) {
        bool processed = false;
        for (const Vector2i& processedWall : processedWalls) {
            if (processedWall.x == w.x && processedWall.y == w.y) {
                processed = true;
                break;
            }
        }

        if (processed) {
            continue;
        }

		sf::RectangleShape rect(Vector2f(16,16));
		rect.setPosition((float)w.x * C::GRID_SIZE, (float)w.y * C::GRID_SIZE);
		rect.setFillColor(sf::Color(0x888888ff));
		wallSprites.push_back(rect);

        processedWalls.push_back(w);
	}
}

void Game::processInput(sf::Event ev) {
	if (ev.type == sf::Event::Closed) {
		win->close();
		closing = true;
		return;
	}
	if (ev.type == sf::Event::KeyReleased) {
		int here = 0;
		if (ev.key.code == Keyboard::K) {
			int there = 0;
			walls.clear();
			cacheWalls();
		}
	}
}


static double g_time = 0.0;
static double g_tickTimer = 0.0;


void Game::pollInput(double dt) {

	float lateralSpeed = 8.0;
	float maxSpeed = 40.0;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
		// PLAYER.squish();
        PLAYER.delta.x -= PLAYER.speed;
		PLAYER.direction &= UP | DOWN | LEFT;
		PLAYER.direction |= LEFT;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		// PLAYER.unsquish();
        PLAYER.delta.x += PLAYER.speed;
		PLAYER.direction &= UP | DOWN | RIGHT;
		PLAYER.direction |= RIGHT;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
        PLAYER.doJump();
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::T) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
        PLAYER.shoot(this->entities);
		if (PLAYER.direction & LEFT) {
			Game::spawnExplosion({ PLAYER.pos.x - 16.0f, PLAYER.pos.y }, 16.0f, true);
		}
		else {
			Game::spawnExplosion({ PLAYER.pos.x + 16.0f, PLAYER.pos.y }, 16.0f, true);
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
		PLAYER.shootHoming(this->entities);
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
		if (!wasPressed) {
			onSpacePressed();
			wasPressed = true;
		}
	}
	else {
		wasPressed = false;
	}

}

static sf::VertexArray va;
static RenderStates vaRs;
static std::vector<sf::RectangleShape> rects;

int blendModeIndex(sf::BlendMode bm) {
	if (bm == sf::BlendAlpha) return 0;
	if (bm == sf::BlendAdd) return 1;
	if (bm == sf::BlendNone) return 2;
	if (bm == sf::BlendMultiply) return 3;
	return 4;
};

void Game::update(double dt) {
	pollInput(dt);

    for (Entity& entity : this->entities) {
        entity.update(this, this->walls, this->entities, dt);
    }

    this->camera.update(dt);
    Tween::update(dt);

    for (const std::string& name : Entity::toDelete) {
        int index = -1;
        int i = 0;
        for (Entity& entity : this->entities) {
            if (entity.NAME == name) {
                index = i;
                break;
            }

            i++;
        }

        if (index > 0) {
            this->entities.erase(this->entities.begin() + index);
        }
    }
    Entity::toDelete.clear();

    for (Explosion& explosion : Game::explosions) {
        if (!explosion.over) {
        	if (explosion.followPlayer) {
        		if (PLAYER.direction & LEFT) {
        			explosion.shape.setPosition({ PLAYER.pos.x - 16.0f, PLAYER.pos.y + 8.0f });
        		}
        		else {
        			explosion.shape.setPosition({ PLAYER.pos.x + 32.0f, PLAYER.pos.y + 8.0f });
        		}
        	}
        	explosion.update(dt);
        }
    }
    
    for (Tween::Tweener& tweener : Tween::tweeners) {
        if (tweener.isOver) {
            tweener.value = nullptr;
        }
    }

	g_time += dt;

	if (bgShader) bgShader->update(dt);

	beforeParts.update(dt);
	afterParts.update(dt);
}

 void Game::draw(sf::RenderWindow & win) {
	if (closing) return;

	sf::RenderStates states = sf::RenderStates::Default;
	sf::Shader * sh = &bgShader->sh;
	states.blendMode = sf::BlendAdd;
	states.shader = sh;
	states.texture = &tex;
	sh->setUniform("texture", tex);
	//sh->setUniform("time", g_time);
	// win.draw(bg, states);

	beforeParts.draw(win);

    if (!this->editor.enabled) {
        this->camera.draw(win);
    }
    else {
        win.setView(win.getDefaultView());
    }

	for (sf::RectangleShape & r : wallSprites)
		win.draw(r);

	for (sf::RectangleShape& r : rects) 
		win.draw(r);
	
	for (Entity& entity : this->entities) {
		entity.draw(win);
	}

	for (Explosion& explosion : Game::explosions) {
		explosion.draw(win);
	}
	
	this->editor.draw(win);

	// afterParts.draw(win);
}

void Game::onSpacePressed() {

}


bool Game::isWall(int cx, int cy)
{
	for (Vector2i & w : walls) {
		if (w.x == cx && w.y == cy)
			return true;
	}
	return false;
}

void Game::im()
{
    if (ImGui::CollapsingHeader("Entities")) {
        for (Entity& entity : this->entities) {
            entity.im();
        }
    }

	if (ImGui::Button("spawn homing projectiel at 10, 54")) {
		this->entities.emplace_back("HOMING", Entity::Type::HomingProjectile, sf::Vector2i(10, 54));
	}

    if (ImGui::CollapsingHeader("Camera")) {
        ImGui::DragFloat("Shake strength", &this->camera.shakeStrength);
        ImGui::DragFloat("Shake time", &this->camera.shakeTime);
        ImGui::DragFloat("Tile width", &this->camera.tileWidth);
    }

    this->editor.im();
    if (this->editor.enabled) {
        this->cacheWalls();
    }
    ImGui::Text("%d walls", this->wallSprites.size());
}

void Game::spawnExplosion(sf::Vector2f pos, float radius, bool followPlayer) {
    for (Explosion& explosion : Game::explosions) {
        if (explosion.over) {
        	explosion.goalRadius = radius;
        	explosion.followPlayer = followPlayer;
            explosion.start(pos);
            return;
        }
    }
}
