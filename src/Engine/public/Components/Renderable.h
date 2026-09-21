#pragma once

#include "Components/Shape.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

struct RenderableComponent
{
	Shape        shape  = Shape::Rectangle;
	sf::Vector2f size{ 0.f, 0.f };
	sf::Color    color  = sf::Color::White;
	sf::Color    outlineColor     = sf::Color::Transparent;
	float        outlineThickness = 0.f;
	const char*  textureId = nullptr; //allows rendering of a textured sprite instead of a solid shape
};
