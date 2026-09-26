#pragma once

#include <SFML/System/Vector2.hpp>

struct MovementComponent
{
	sf::Vector2f velocity{ 0.f, 0.f };
	// Intrinsic move speed for input-driven entities: the rate their position is advanced per
	// second. Self-propelled entities ignore it and drive `velocity` directly.
	float speed = 0.f;
};
