#pragma once

#include <SFML/System/Vector2.hpp>

struct MovementComponent
{
	sf::Vector2f velocity{ 0.f, 0.f };
	// Intrinsic move speed for input-driven entities (the paddle): the rate its position is
	// advanced per second. Self-propelled entities (ball, powerup, laser) ignore it and drive
	// `velocity` directly. Effects (Weight of Sin) scale this in place.
	float speed = 0.f;
};
