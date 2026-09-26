#include "ECS/EntityManager.h"

#include <gtest/gtest.h>

// EntityManager is a process-wide singleton: each test uses its own component type so pools
// never leak state between tests.
namespace
{
struct PositionA { int x = 0; };
struct PositionB { int x = 0; };
struct PositionC { int x = 0; };
struct TagD {};
struct PositionE { int x = 0; };
} // namespace

TEST(EntityManager, CreateEntityReturnsDistinctIds)
{
	auto& entities = EntityManager::Get();
	const DEPRECATED_EntityId a = entities.CreateEntity();
	const DEPRECATED_EntityId b = entities.CreateEntity();
	EXPECT_NE(a, b);
}

TEST(EntityManager, AddedComponentIsRetrievable)
{
	auto& entities = EntityManager::Get();
	const DEPRECATED_EntityId entity = entities.CreateEntity();
	entities.AddComponent(entity, PositionA{ 42 });

	const PositionA* position = entities.GetComponent<PositionA>(entity);
	ASSERT_NE(position, nullptr);
	EXPECT_EQ(position->x, 42);
}

TEST(EntityManager, GetComponentReturnsNullWhenMissing)
{
	auto& entities = EntityManager::Get();
	const DEPRECATED_EntityId entity = entities.CreateEntity();
	EXPECT_EQ(entities.GetComponent<PositionB>(entity), nullptr);
}

TEST(EntityManager, DestroyEntityRemovesItsComponents)
{
	auto& entities = EntityManager::Get();
	const DEPRECATED_EntityId entity = entities.CreateEntity();
	entities.AddComponent(entity, PositionC{ 1 });

	entities.DestroyEntity(entity);

	EXPECT_EQ(entities.GetComponent<PositionC>(entity), nullptr);
}

TEST(EntityManager, DestroyAllEntitiesWithComponentOnlyTouchesTaggedEntities)
{
	auto& entities = EntityManager::Get();
	const DEPRECATED_EntityId tagged = entities.CreateEntity();
	const DEPRECATED_EntityId untagged = entities.CreateEntity();
	entities.AddComponent<TagD>(tagged);
	entities.AddComponent(tagged, PositionE{ 1 });
	entities.AddComponent(untagged, PositionE{ 2 });

	entities.DestroyAllEntitiesWithComponent<TagD>();

	EXPECT_EQ(entities.GetComponent<PositionE>(tagged), nullptr);
	ASSERT_NE(entities.GetComponent<PositionE>(untagged), nullptr);
	EXPECT_EQ(entities.GetComponent<PositionE>(untagged)->x, 2);
}

TEST(EntityManager, CloneEntityCopiesComponents)
{
	struct Health { int value = 0; };

	auto& entities = EntityManager::Get();
	const DEPRECATED_EntityId source = entities.CreateEntity();
	entities.AddComponent(source, Health{ 7 });

	const DEPRECATED_EntityId clone = entities.CloneEntity(source);

	EXPECT_NE(clone, source);
	ASSERT_NE(entities.GetComponent<Health>(clone), nullptr);
	EXPECT_EQ(entities.GetComponent<Health>(clone)->value, 7);

	// The copy is independent from the source.
	entities.GetComponent<Health>(clone)->value = 3;
	EXPECT_EQ(entities.GetComponent<Health>(source)->value, 7);
}
