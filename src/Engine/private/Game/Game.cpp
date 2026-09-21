#include "Game/Game.h"

#include "Systems/System.h"

#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

Game::Game(unsigned int width, unsigned int height, const std::string& title)
    : m_window(sf::VideoMode({ width, height }), title)
{
}

int Game::Run()
{
	OnStart();

	sf::Clock clock;
	while (m_window.isOpen())
	{
		while (const auto event = m_window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				m_window.close();
			else
				OnEvent(*event);
		}

		Update(clock.restart().asSeconds());

		Render(m_window);
		m_window.display();
	}
	return 0;
}

void Game::Update(float deltaTime)
{
	for (ISystem* system : m_systems)
		system->Update(deltaTime);
}

void Game::Render(sf::RenderWindow& window)
{
	window.clear();
	m_render.Render(window);
}
