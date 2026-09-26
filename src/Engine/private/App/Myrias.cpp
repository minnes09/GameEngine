#include "App/Myrias.h"

#include "ECS/EntityManager.h"
#include "ECS/Systems/System.h"
#include "Renderer/AssetManager.h"
#include "Renderer/RenderExtraction.h"
#include "Renderer/Renderer.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

Myrias::Myrias(const MyriasConfig& config)
    : window(std::make_unique<sf::RenderWindow>(sf::VideoMode({ config.width, config.height }), config.title))
    , renderer(std::make_unique<Renderer>(*window, AssetManager::Get()))
{
}

Myrias::~Myrias() = default;

int Myrias::Run()
{
	sf::Clock clock;
	while (window->isOpen())
	{
		while (const auto event = window->pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window->close();
		}

		Update(clock.restart().asSeconds());

		renderer->BeginFrame(clearColor);
		Render(renderer->Queue());
		renderer->EndFrame();
		window->display();
	}
	return 0;
}

void Myrias::Update(float deltaTime)
{
	for (ISystem* system : systems)
		system->Update(deltaTime);
}

void Myrias::Render(RenderQueue& queue)
{
	ExtractRenderQueue(EntityManager::Get(), queue);
}
