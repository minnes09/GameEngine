#include "Renderer/RenderExtraction.h"
#include "Renderer/RenderQueue.h"

#include "ECS/ComponentPool.h"
#include "ECS/Components/Renderable.h"
#include "ECS/Components/Text.h"
#include "ECS/Components/Transform.h"
#include "Support/AllocationCounter.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

// Principle 4 (frame without allocations), verified directly: steady-state frames must not touch the heap.

TEST(RenderAllocations, CounterDetectsAllocations)
{
	std::size_t count = 0;
	{
		const AllocationScope scope;
		std::vector<int>      values;
		values.reserve(64);
		values.push_back(1);
		count = scope.Count();
	}
	EXPECT_GE(count, 1u);
}

TEST(RenderAllocations, SteadyStateQueueFrameDoesNotAllocate)
{
	auto fillFrame = [](RenderQueue& queue)
	{
		for (int i = 0; i < 64; ++i)
		{
			queue.PushShape({ .position = { static_cast<float>(i), 0.f } });
			queue.PushText({ .text = "steady state label" });
		}
	};

	RenderQueue queue;
	fillFrame(queue); // warm-up: capacities grow here

	std::size_t count = 0;
	{
		const AllocationScope scope;
		queue.Clear();
		fillFrame(queue);
		for (std::size_t i = 0; i < queue.TextCount(); ++i)
			static_cast<void>(queue.Text(i));
		count = scope.Count();
	}
	EXPECT_EQ(count, 0u);
}

TEST(RenderAllocations, SteadyStateExtractionDoesNotAllocate)
{
	ComponentPool<TransformComponent>  transforms;
	ComponentPool<RenderableComponent> renderables;
	ComponentPool<TextComponent>       texts;
	for (std::uint32_t i = 0; i < 64; ++i)
	{
		transforms.Add(DEPRECATED_EntityId{ i }, {});
		renderables.Add(DEPRECATED_EntityId{ i }, {});
		if (i % 4 == 0)
			texts.Add(DEPRECATED_EntityId{ i }, { .text = "label" });
	}

	RenderQueue queue;
	ExtractRenderQueue(transforms, renderables, &texts, queue); // warm-up

	std::size_t count = 0;
	{
		const AllocationScope scope;
		queue.Clear();
		ExtractRenderQueue(transforms, renderables, &texts, queue);
		count = scope.Count();
	}
	EXPECT_EQ(count, 0u);
}
