#include "Physics/CollisionGeometry.h"

#include <gtest/gtest.h>

namespace
{
constexpr float kEpsilon = 1e-4f;

CollisionComponent MakeCollider(Shape shape, sf::Vector2f min, sf::Vector2f max)
{
	CollisionComponent collider;
	collider.shape = shape;
	collider.size  = max - min;
	collider.min   = min;
	collider.max   = max;
	return collider;
}
} // namespace

TEST(CollisionGeometry, RectVsRectSeparatedHasNoContact)
{
	const Contact contact = CollisionGeometry::RectVsRect({ 0.f, 0.f }, { 1.f, 1.f }, { 2.f, 0.f }, { 3.f, 1.f });
	EXPECT_FALSE(contact.hit);
}

TEST(CollisionGeometry, RectVsRectResolvesAlongSmallestOverlap)
{
	// a overlaps b by 0.25 on X and fully on Y: separation is along X, pushing a to the left.
	const Contact contact = CollisionGeometry::RectVsRect({ 0.f, 0.f }, { 1.f, 1.f }, { 0.75f, 0.f }, { 2.f, 1.f });
	ASSERT_TRUE(contact.hit);
	EXPECT_NEAR(contact.penetration, 0.25f, kEpsilon);
	EXPECT_NEAR(contact.normal.x, -1.f, kEpsilon);
	EXPECT_NEAR(contact.normal.y, 0.f, kEpsilon);
}

TEST(CollisionGeometry, CircleVsCircleOverlapping)
{
	const Contact contact = CollisionGeometry::CircleVsCircle({ 0.f, 0.f }, 1.f, { 1.5f, 0.f }, 1.f);
	ASSERT_TRUE(contact.hit);
	EXPECT_NEAR(contact.penetration, 0.5f, kEpsilon);
	EXPECT_NEAR(contact.normal.x, -1.f, kEpsilon); // from b into a
}

TEST(CollisionGeometry, CircleVsCircleTouchingIsNotAContact)
{
	const Contact contact = CollisionGeometry::CircleVsCircle({ 0.f, 0.f }, 1.f, { 2.f, 0.f }, 1.f);
	EXPECT_FALSE(contact.hit);
}

TEST(CollisionGeometry, CircleCenterInsideRectPushesOutThroughNearestEdge)
{
	// Center 0.1 from the left edge of a 10x10 box.
	const Contact contact = CollisionGeometry::CircleVsRect({ 0.1f, 5.f }, 1.f, { 0.f, 0.f }, { 10.f, 10.f });
	ASSERT_TRUE(contact.hit);
	EXPECT_NEAR(contact.penetration, 1.1f, kEpsilon);
	EXPECT_NEAR(contact.normal.x, -1.f, kEpsilon);
}

TEST(CollisionGeometry, TestContactRectVsCircleFlipsNormal)
{
	const CollisionComponent rect   = MakeCollider(Shape::Rectangle, { 0.f, 0.f }, { 2.f, 2.f });
	const CollisionComponent circle = MakeCollider(Shape::Circle, { 1.5f, 0.5f }, { 2.5f, 1.5f });

	const Contact rectFirst   = CollisionGeometry::TestContact(rect, circle);
	const Contact circleFirst = CollisionGeometry::TestContact(circle, rect);

	ASSERT_TRUE(rectFirst.hit);
	ASSERT_TRUE(circleFirst.hit);
	EXPECT_NEAR(rectFirst.normal.x, -circleFirst.normal.x, kEpsilon);
	EXPECT_NEAR(rectFirst.normal.y, -circleFirst.normal.y, kEpsilon);
}
