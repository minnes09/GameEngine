#pragma once

#include "ECS/Systems/RenderSystem.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include <string>
#include <vector>

class ISystem;
namespace sf { class Event; }

// Minimal, generic runnable base: opens a window and drives the event -> update -> render loop.
// A game builds on top by subclassing and overriding the lifecycle hooks. The engine knows nothing
// game-specific — state machines, levels, debug, etc. live entirely in the game's overrides.
class Game
{
public:
	Game(unsigned int width, unsigned int height, const std::string& title);
	virtual ~Game() = default;
	Game(const Game&)            = delete;
	Game& operator=(const Game&) = delete;

	int Run();

	// Registers a system stepped by the default Update() in registration order. Non-owning:
	// the systems are owned by the game. Ignored if the game overrides Update().
	void AddSystem(ISystem& system) { systems.push_back(&system); }

	sf::RenderWindow& GetWindow() { return window; }

protected:
	virtual void OnStart() {}                             // setup: load assets, create entities, ...
	virtual void Update(float deltaTime);                // default: step the registered systems
	virtual void Render(sf::RenderWindow& target);       // default: clear + RenderSystem
	virtual void OnEvent(const sf::Event& /*event*/) {}   // default: ignore (Close is handled by Run)

private:
	sf::RenderWindow      window;
	std::vector<ISystem*> systems;
	RenderSystem          renderSystem;
};
