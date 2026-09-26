#include "Renderer/RenderQueue.h"

#include <gtest/gtest.h>

#include <string>

TEST(RenderQueue, StartsEmpty)
{
	const RenderQueue queue;
	EXPECT_TRUE(queue.Shapes().empty());
	EXPECT_EQ(queue.TextCount(), 0u);
}

TEST(RenderQueue, PushedCommandsKeepInsertionOrder)
{
	RenderQueue queue;
	queue.PushShape({ .kind = ShapeKind::Rectangle, .position = { 1.f, 2.f } });
	queue.PushShape({ .kind = ShapeKind::Circle, .position = { 3.f, 4.f } });
	queue.PushText({ .text = "first" });
	queue.PushText({ .text = "second" });

	ASSERT_EQ(queue.Shapes().size(), 2u);
	EXPECT_EQ(queue.Shapes()[0].kind, ShapeKind::Rectangle);
	EXPECT_EQ(queue.Shapes()[1].kind, ShapeKind::Circle);
	EXPECT_FLOAT_EQ(queue.Shapes()[1].position.x, 3.f);

	ASSERT_EQ(queue.TextCount(), 2u);
	EXPECT_EQ(queue.Text(0).text, "first");
	EXPECT_EQ(queue.Text(1).text, "second");
}

TEST(RenderQueue, ClearEmptiesButKeepsCapacity)
{
	RenderQueue queue;
	queue.Reserve(8, 4);
	for (int i = 0; i < 8; ++i)
		queue.PushShape({});
	for (int i = 0; i < 4; ++i)
		queue.PushText({});

	const std::size_t shapeCapacity = queue.ShapeCapacity();
	const std::size_t textCapacity  = queue.TextCapacity();

	queue.Clear();

	EXPECT_TRUE(queue.Shapes().empty());
	EXPECT_EQ(queue.TextCount(), 0u);
	EXPECT_EQ(queue.ShapeCapacity(), shapeCapacity);
	EXPECT_EQ(queue.TextCapacity(), textCapacity);
}

TEST(RenderQueue, RefillWithinCapacityDoesNotGrow)
{
	RenderQueue queue;
	queue.Reserve(16, 16);
	const std::size_t shapeCapacity = queue.ShapeCapacity();

	for (int frame = 0; frame < 3; ++frame)
	{
		queue.Clear();
		for (int i = 0; i < 16; ++i)
			queue.PushShape({});
	}

	EXPECT_EQ(queue.ShapeCapacity(), shapeCapacity);
}

TEST(RenderQueue, PushedTextOutlivesItsSource)
{
	RenderQueue queue;
	{
		std::string temporary = "Score 42";
		queue.PushText({ .text = temporary });
		temporary = "overwritten";
	}
	ASSERT_EQ(queue.TextCount(), 1u);
	EXPECT_EQ(queue.Text(0).text, "Score 42");
}

TEST(RenderQueue, TextsStayIntactWhenBufferGrows)
{
	RenderQueue queue;
	queue.PushText({ .text = "first" });
	for (int i = 0; i < 100; ++i)
		queue.PushText({ .text = "a longer string that forces the buffer to reallocate" });

	EXPECT_EQ(queue.Text(0).text, "first");
	EXPECT_EQ(queue.Text(100).text, "a longer string that forces the buffer to reallocate");
}

TEST(RenderQueue, ClearKeepsTextBufferCapacity)
{
	RenderQueue queue;
	queue.Reserve(0, 4, 256);
	queue.PushText({ .text = "some text" });
	const std::size_t bufferCapacity = queue.TextBufferCapacity();

	queue.Clear();

	EXPECT_EQ(queue.TextCount(), 0u);
	EXPECT_EQ(queue.TextBufferCapacity(), bufferCapacity);
}
