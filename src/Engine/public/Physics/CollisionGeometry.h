#pragma once

#include "ECS/Components/Collision.h"

#include <SFML/System/Vector2.hpp>

// Result of a narrowphase test. The normal points from `b` into `a` (the direction `a` would have
// to move to separate). Default (hit == false) means "no contact".
struct Contact
{
	bool         hit         = false;
	float        penetration = 0.f;
	sf::Vector2f normal{ 0.f, 0.f };
};

// Engine-generic AABB/circle narrowphase. Operates on the collider's cached AABB (min/max) and
// shape; carries no game knowledge (layers/response live in the game's collision system).
namespace CollisionGeometry
{
// A Circle is the inscribed circle of its bounding box, so its radius is half the box width.
float Radius(const CollisionComponent& collision);

Contact CircleVsRect(sf::Vector2f circleCenter, float radius, sf::Vector2f rectMin, sf::Vector2f rectMax);
Contact CircleVsCircle(sf::Vector2f centerA, float radiusA, sf::Vector2f centerB, float radiusB);
Contact RectVsRect(sf::Vector2f aMin, sf::Vector2f aMax, sf::Vector2f bMin, sf::Vector2f bMax);

// Dispatches on the pair's shape combination. The Contact's normal always points from `b` into `a`.
Contact TestContact(const CollisionComponent& a, const CollisionComponent& b);
} // namespace CollisionGeometry
