#pragma once

#include "Renderer/RenderTypes.h"

#include <memory>
#include <string>
#include <vector>

class ISystem;
class Renderer;
class RenderQueue;
namespace sf
{
class RenderWindow;
class Event;
}

// Minimal, generic application base: opens a window and drives the event -> update -> render loop.
// A game builds on top by subclassing and overriding the lifecycle hooks. The engine knows nothing
// game-specific: state machines, levels, debug, etc. live entirely in the game's overrides.
class Aion
{
public:
	Aion(unsigned int width, unsigned int height, const std::string& title);
	virtual ~Aion();
	Aion(const Aion&)            = delete;
	Aion& operator=(const Aion&) = delete;

	int Run();

	// Registers a system stepped by the default Update() in registration order. Non-owning:
	// the systems are owned by the game. Ignored if the game overrides Update().
	void AddSystem(ISystem& system) { systems.push_back(&system); }

	void SetClearColor(RenderColor color) noexcept { clearColor = color; }

	// Temporary until the Platform module exists.
	sf::RenderWindow& GetWindow() noexcept { return *window; }

protected:
	virtual void OnStart() {}                          // setup: load assets, create entities, ...
	virtual void Update(float deltaTime);             // default: step the registered systems
	virtual void Render(RenderQueue& queue);          // default: extract the ECS render components
	virtual void OnEvent(const sf::Event& /*event*/) {} // default: ignore (Close is handled by Run). Temporary until Platform.

private:
	std::unique_ptr<sf::RenderWindow> window;
	std::unique_ptr<Renderer>         renderer; // declared after window: destroyed before it
	std::vector<ISystem*>             systems;
	RenderColor                       clearColor{ 0, 0, 0, 255 };
};
