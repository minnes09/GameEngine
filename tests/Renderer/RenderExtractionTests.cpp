#include "Renderer/RenderExtraction.h"

#include "ECS/ComponentPool.h"
#include "ECS/Components/Renderable.h"
#include "ECS/Components/Text.h"
#include "ECS/Components/Transform.h"
#include "ECS/EntityManager.h"
#include "Renderer/RenderQueue.h"

#include <gtest/gtest.h>

// Uses local pools, not the EntityManager singleton, so tests are independent.
namespace
{
constexpr EntityId kFirst{ 1 };
constexpr EntityId kSecond{ 2 };

struct Pools
{
	ComponentPool<TransformComponent>  transforms;
	ComponentPool<RenderableComponent> renderables;
	ComponentPool<TextComponent>       texts;
};
} // namespace

TEST(RenderExtraction, EmptyPoolsProduceEmptyQueue)
{
	Pools       pools;
	RenderQueue queue;
	ExtractRenderQueue(pools.transforms, pools.renderables, &pools.texts, queue);
	EXPECT_TRUE(queue.Shapes().empty());
	EXPECT_EQ(queue.TextCount(), 0u);
}

TEST(RenderExtraction, RectangleCopiesAllFields)
{
	static const char kTexture[] = "tex";

	Pools pools;
	pools.transforms.Add(kFirst, { .position = { 10.f, 20.f } });
	pools.renderables.Add(kFirst, { .shape            = Shape::Rectangle,
	                                .size             = { 30.f, 40.f },
	                                .color            = sf::Color::Red,
	                                .outlineColor     = sf::Color::Blue,
	                                .outlineThickness = 2.f,
	                                .textureId        = kTexture });

	RenderQueue queue;
	ExtractRenderQueue(pools.transforms, pools.renderables, nullptr, queue);

	ASSERT_EQ(queue.Shapes().size(), 1u);
	const ShapeDrawCommand& shape = queue.Shapes()[0];
	EXPECT_EQ(shape.kind, ShapeKind::Rectangle);
	EXPECT_EQ(shape.position, RenderVec2(10.f, 20.f));
	EXPECT_EQ(shape.size, RenderVec2(30.f, 40.f));
	EXPECT_EQ(shape.fillColor, sf::Color::Red);
	EXPECT_EQ(shape.outlineColor, sf::Color::Blue);
	EXPECT_FLOAT_EQ(shape.outlineThickness, 2.f);
	EXPECT_EQ(shape.textureId, kTexture); // same pointer, not a copy
}

TEST(RenderExtraction, CircleMapsToCircleKind)
{
	Pools pools;
	pools.transforms.Add(kFirst, {});
	pools.renderables.Add(kFirst, { .shape = Shape::Circle, .size = { 8.f, 8.f } });

	RenderQueue queue;
	ExtractRenderQueue(pools.transforms, pools.renderables, nullptr, queue);

	ASSERT_EQ(queue.Shapes().size(), 1u);
	EXPECT_EQ(queue.Shapes()[0].kind, ShapeKind::Circle);
}

TEST(RenderExtraction, RenderableWithoutTransformIsSkipped)
{
	Pools pools;
	pools.renderables.Add(kFirst, {});
	pools.transforms.Add(kSecond, {});
	pools.renderables.Add(kSecond, {});

	RenderQueue queue;
	ExtractRenderQueue(pools.transforms, pools.renderables, nullptr, queue);

	EXPECT_EQ(queue.Shapes().size(), 1u);
}

TEST(RenderExtraction, TextIsCenteredInRenderableBox)
{
	Pools pools;
	pools.transforms.Add(kFirst, { .position = { 100.f, 50.f } });
	pools.renderables.Add(kFirst, { .size = { 20.f, 10.f } });
	pools.texts.Add(kFirst, { .text = "hi", .characterSize = 24, .color = sf::Color::Green });

	RenderQueue queue;
	ExtractRenderQueue(pools.transforms, pools.renderables, &pools.texts, queue);

	ASSERT_EQ(queue.TextCount(), 1u);
	const TextDrawCommand text = queue.Text(0);
	EXPECT_EQ(text.center, RenderVec2(110.f, 55.f));
	EXPECT_EQ(text.text, "hi");
	EXPECT_EQ(text.characterSize, 24u);
	EXPECT_EQ(text.color, sf::Color::Green);
}

TEST(RenderExtraction, TextWithoutRenderableUsesPosition)
{
	Pools pools;
	pools.transforms.Add(kFirst, { .position = { 7.f, 9.f } });
	pools.texts.Add(kFirst, { .text = "label" });

	RenderQueue queue;
	ExtractRenderQueue(pools.transforms, pools.renderables, &pools.texts, queue);

	ASSERT_EQ(queue.TextCount(), 1u);
	EXPECT_EQ(queue.Text(0).center, RenderVec2(7.f, 9.f));
}

TEST(RenderExtraction, TextWithoutTransformIsSkipped)
{
	Pools pools;
	pools.texts.Add(kFirst, { .text = "orphan" });

	RenderQueue queue;
	ExtractRenderQueue(pools.transforms, pools.renderables, &pools.texts, queue);

	EXPECT_EQ(queue.TextCount(), 0u);
}

TEST(RenderExtraction, NullTextPoolSkipsTextPass)
{
	Pools pools;
	pools.transforms.Add(kFirst, {});
	pools.texts.Add(kFirst, { .text = "ignored" });

	RenderQueue queue;
	ExtractRenderQueue(pools.transforms, pools.renderables, nullptr, queue);

	EXPECT_EQ(queue.TextCount(), 0u);
}

TEST(RenderExtraction, TextIsCopiedIntoQueue)
{
	Pools pools;
	pools.transforms.Add(kFirst, {});
	TextComponent& component = pools.texts.Add(kFirst, { .text = "original" });

	RenderQueue queue;
	ExtractRenderQueue(pools.transforms, pools.renderables, &pools.texts, queue);
	component.text = "changed after extraction";

	ASSERT_EQ(queue.TextCount(), 1u);
	EXPECT_EQ(queue.Text(0).text, "original");
}

TEST(RenderExtraction, EntityManagerOverloadDrawsTextWithoutRenderablePool)
{
	// Uses the singleton: the text is unique to this test. The branch under test needs the
	// Renderable pool to be absent.
	auto& entities = EntityManager::Get();
	if (entities.GetPool<RenderableComponent>())
		GTEST_SKIP() << "Renderable pool already exists in the EntityManager singleton";
	const EntityId entity = entities.CreateEntity();
	entities.AddComponent(entity, TransformComponent{ .position = { 3.f, 4.f } });
	entities.AddComponent(entity, TextComponent{ .text = "RenderExtractionTests.text-only" });

	RenderQueue queue;
	ExtractRenderQueue(entities, queue);

	bool bFound = false;
	for (std::size_t i = 0; i < queue.TextCount(); ++i)
		bFound = bFound || queue.Text(i).text == "RenderExtractionTests.text-only";
	EXPECT_TRUE(bFound);

	entities.DestroyEntity(entity);
}

TEST(RenderExtraction, AppendsWithoutClearing)
{
	Pools pools;
	pools.transforms.Add(kFirst, {});
	pools.renderables.Add(kFirst, {});

	RenderQueue queue;
	ExtractRenderQueue(pools.transforms, pools.renderables, nullptr, queue);
	ExtractRenderQueue(pools.transforms, pools.renderables, nullptr, queue);

	EXPECT_EQ(queue.Shapes().size(), 2u);
}

TEST(RenderExtraction, RepeatedFramesDoNotGrowQueue)
{
	Pools pools;
	for (std::uint32_t i = 0; i < 32; ++i)
	{
		pools.transforms.Add(EntityId{ i }, {});
		pools.renderables.Add(EntityId{ i }, {});
		pools.texts.Add(EntityId{ i }, { .text = "t" });
	}

	RenderQueue queue;
	ExtractRenderQueue(pools.transforms, pools.renderables, &pools.texts, queue);
	const std::size_t shapeCapacity = queue.ShapeCapacity();
	const std::size_t textCapacity       = queue.TextCapacity();
	const std::size_t textBufferCapacity = queue.TextBufferCapacity();

	for (int frame = 0; frame < 3; ++frame)
	{
		queue.Clear();
		ExtractRenderQueue(pools.transforms, pools.renderables, &pools.texts, queue);
	}

	EXPECT_EQ(queue.ShapeCapacity(), shapeCapacity);
	EXPECT_EQ(queue.TextCapacity(), textCapacity);
	EXPECT_EQ(queue.TextBufferCapacity(), textBufferCapacity);
}
