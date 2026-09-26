#pragma once

#include "ECS/Components/Shape.h"

#include <SFML/System/Vector2.hpp>

#include <cstdint>

struct CollisionComponent
{
	Shape        shape = Shape::Rectangle;
	sf::Vector2f size{ 0.f, 0.f };
	// Generic collision layer id. Games map their own channel enum onto this (the numeric value
	// of the enumerator is the layer). Keeps the collider engine-generic.
	std::uint8_t layer = 0;

	// Cached AABB, refreshed from the entity's Transform each frame.
	sf::Vector2f min{ 0.f, 0.f };
	sf::Vector2f max{ 0.f, 0.f };
};
