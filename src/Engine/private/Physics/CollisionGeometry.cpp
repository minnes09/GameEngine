#include "Physics/CollisionGeometry.h"

#include "ECS/Components/Shape.h"

#include <algorithm>
#include <cmath>

namespace CollisionGeometry
{
float Radius(const CollisionComponent& collision)
{
	return collision.size.x * 0.5f;
}

Contact CircleVsRect(sf::Vector2f circleCenter, float radius,
                     sf::Vector2f rectMin, sf::Vector2f rectMax)
{
	const sf::Vector2f closest{ std::clamp(circleCenter.x, rectMin.x, rectMax.x),
	                            std::clamp(circleCenter.y, rectMin.y, rectMax.y) };
	const sf::Vector2f delta = circleCenter - closest;
	const float        dist2 = delta.x * delta.x + delta.y * delta.y;
	if (dist2 >= radius * radius)
		return {};

	// Circle center inside the rect: push out along the nearest edge.
	if (dist2 < 1e-6f)
	{
		const float distLeft   = circleCenter.x - rectMin.x;
		const float distRight  = rectMax.x - circleCenter.x;
		const float distTop    = circleCenter.y - rectMin.y;
		const float distBottom = rectMax.y - circleCenter.y;
		const float minDist    = std::min({ distLeft, distRight, distTop, distBottom });
		if (minDist == distLeft)   return { true, radius + distLeft,   { -1.f,  0.f } };
		if (minDist == distRight)  return { true, radius + distRight,  {  1.f,  0.f } };
		if (minDist == distTop)    return { true, radius + distTop,    {  0.f, -1.f } };
		return                          { true, radius + distBottom, {  0.f,  1.f } };
	}

	const float dist = std::sqrt(dist2);
	return { true, radius - dist, delta / dist };
}

Contact CircleVsCircle(sf::Vector2f centerA, float radiusA,
                       sf::Vector2f centerB, float radiusB)
{
	const sf::Vector2f delta    = centerA - centerB;
	const float        dist2    = delta.x * delta.x + delta.y * delta.y;
	const float        radiiSum = radiusA + radiusB;
	if (dist2 >= radiiSum * radiiSum)
		return {};
	if (dist2 < 1e-6f)
		return { true, radiiSum, { 0.f, -1.f } };
	const float dist = std::sqrt(dist2);
	return { true, radiiSum - dist, delta / dist };
}

Contact RectVsRect(sf::Vector2f aMin, sf::Vector2f aMax,
                   sf::Vector2f bMin, sf::Vector2f bMax)
{
	if (!(aMin.x < bMax.x && aMax.x > bMin.x && aMin.y < bMax.y && aMax.y > bMin.y))
		return {};
	const float overlapX = std::min(aMax.x, bMax.x) - std::max(aMin.x, bMin.x);
	const float overlapY = std::min(aMax.y, bMax.y) - std::max(aMin.y, bMin.y);
	const sf::Vector2f centerA = (aMin + aMax) * 0.5f;
	const sf::Vector2f centerB = (bMin + bMax) * 0.5f;
	if (overlapX < overlapY)
		return { true, overlapX, { centerA.x < centerB.x ? -1.f : 1.f, 0.f } };
	return     { true, overlapY, { 0.f, centerA.y < centerB.y ? -1.f : 1.f } };
}

Contact TestContact(const CollisionComponent& a, const CollisionComponent& b)
{
	const sf::Vector2f centerA = (a.min + a.max) * 0.5f;
	const sf::Vector2f centerB = (b.min + b.max) * 0.5f;

	if (a.shape == Shape::Circle && b.shape == Shape::Circle)
		return CircleVsCircle(centerA, Radius(a), centerB, Radius(b));

	if (a.shape == Shape::Circle && b.shape == Shape::Rectangle)
		return CircleVsRect(centerA, Radius(a), b.min, b.max);

	if (a.shape == Shape::Rectangle && b.shape == Shape::Circle)
	{
		Contact contact = CircleVsRect(centerB, Radius(b), a.min, a.max);
		contact.normal  = -contact.normal; // re-orient from b->a to a->b
		return contact;
	}

	return RectVsRect(a.min, a.max, b.min, b.max);
}
} // namespace CollisionGeometry
