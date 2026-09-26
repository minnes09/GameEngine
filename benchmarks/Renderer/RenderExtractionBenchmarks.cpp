// ECS -> RenderQueue extraction over N renderables (10% with text). Uses local pools, not the singleton.
// Covers violations #11 (join via hash lookup) in architecture.md. Run in Release only.

#include "ECS/ComponentPool.h"
#include "ECS/Components/Renderable.h"
#include "ECS/Components/Text.h"
#include "ECS/Components/Transform.h"
#include "Renderer/RenderExtraction.h"
#include "Renderer/RenderQueue.h"

#include <benchmark/benchmark.h>

#include <cstdint>

static void RenderExtraction(benchmark::State& state)
{
	const auto count = static_cast<std::uint32_t>(state.range(0));

	ComponentPool<TransformComponent>  transforms;
	ComponentPool<RenderableComponent> renderables;
	ComponentPool<TextComponent>       texts;
	for (std::uint32_t i = 0; i < count; ++i)
	{
		const DEPRECATED_EntityId entity{ i };
		transforms.Add(entity, { .position = { static_cast<float>(i), 0.f } });
		renderables.Add(entity, { .size = { 8.f, 8.f } });
		if (i % 10 == 0)
			texts.Add(entity, { .text = "label" });
	}

	RenderQueue queue;
	ExtractRenderQueue(transforms, renderables, &texts, queue); // warm-up: capacities grow once

	for (auto _ : state)
	{
		queue.Clear();
		ExtractRenderQueue(transforms, renderables, &texts, queue);
		benchmark::DoNotOptimize(queue.Shapes().data());
	}
	state.SetItemsProcessed(state.iterations() * state.range(0));
}

BENCHMARK(RenderExtraction)->Arg(1'000)->Arg(10'000)->Arg(100'000)->Unit(benchmark::kMicrosecond);
