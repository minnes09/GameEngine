#pragma once

#include <SFML/Graphics/Color.hpp>

#include <string>

struct TextComponent
{
	std::string  text;
	unsigned int characterSize = 16;
	sf::Color    color         = sf::Color::White;
};
