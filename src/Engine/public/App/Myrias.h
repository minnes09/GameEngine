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
}

struct MyriasConfig
{
	unsigned int width  = 1280;
	unsigned int height = 720;
	std::string  title  = "Myrias";
};

// The engine application: owns the window and the renderer and drives the
// event -> update -> render loop. Games configure it (systems, entities) instead of
// subclassing it: all game behaviour lives in systems.
class Myrias final
{
public:
	explicit Myrias(const MyriasConfig& config);
	~Myrias();
	Myrias(const Myrias&)            = delete;
	Myrias& operator=(const Myrias&) = delete;

	// Runs until the window is closed.
	int Run();

	// Registers a system stepped every update in registration order. Non-owning: the system
	// must outlive Run().
	void AddSystem(ISystem& system) { systems.push_back(&system); }

	void SetClearColor(RenderColor color) noexcept { clearColor = color; }

	// Temporary until the Platform module exists.
	sf::RenderWindow& GetWindow() noexcept { return *window; }

private:
	void Update(float deltaTime);
	void Render(RenderQueue& queue);

	std::unique_ptr<sf::RenderWindow> window;
	std::unique_ptr<Renderer>         renderer; // declared after window: destroyed before it
	std::vector<ISystem*>             systems;
	RenderColor                       clearColor{ 0, 0, 0, 255 };
};
