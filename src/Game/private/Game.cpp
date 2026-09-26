#include "Game.h"

#include "ECS/Systems/System.h"

#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

Game::Game(unsigned int width, unsigned int height, const std::string& title)
    : window(sf::VideoMode({ width, height }), title)
{
}

int Game::Run()
{
	OnStart();

	sf::Clock clock;
	while (window.isOpen())
	{
		while (const auto event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();
			else
				OnEvent(*event);
		}

		Update(clock.restart().asSeconds());

		Render(window);
		window.display();
	}
	return 0;
}

void Game::Update(float deltaTime)
{
	for (ISystem* system : systems)
		system->Update(deltaTime);
}

void Game::Render(sf::RenderWindow& target)
{
	target.clear();
	renderSystem.Render(target);
}
