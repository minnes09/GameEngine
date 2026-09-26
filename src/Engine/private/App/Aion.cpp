#include "App/Aion.h"

#include "ECS/EntityManager.h"
#include "ECS/Systems/System.h"
#include "Renderer/AssetManager.h"
#include "Renderer/RenderExtraction.h"
#include "Renderer/Renderer.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

Aion::Aion(unsigned int width, unsigned int height, const std::string& title)
    : window(std::make_unique<sf::RenderWindow>(sf::VideoMode({ width, height }), title))
    , renderer(std::make_unique<Renderer>(*window, AssetManager::Get()))
{
}

Aion::~Aion() = default;

int Aion::Run()
{
	OnStart();

	sf::Clock clock;
	while (window->isOpen())
	{
		while (const auto event = window->pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window->close();
			else
				OnEvent(*event);
		}

		Update(clock.restart().asSeconds());

		renderer->BeginFrame(clearColor);
		Render(renderer->Queue());
		renderer->EndFrame();
		window->display();
	}
	return 0;
}

void Aion::Update(float deltaTime)
{
	for (ISystem* system : systems)
		system->Update(deltaTime);
}

void Aion::Render(RenderQueue& queue)
{
	ExtractRenderQueue(EntityManager::Get(), queue);
}
