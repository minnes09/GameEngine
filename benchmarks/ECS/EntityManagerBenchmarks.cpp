// Baseline for the ECS storage (violations #10-#14 in architecture.md). Run in Release only.
//
// EntityManager is a singleton without reset: every benchmark uses its own component types
// (BenchData<Tag>) and destroys its entities at the end, so benchmarks do not see each other's pools.

#include "ECS/ComponentPool.h"
#include "ECS/EntityManager.h"

#include <benchmark/benchmark.h>

#include <cstdint>
#include <utility>
#include <vector>

namespace
{
template <int Tag>
struct BenchData
{
	float x = 0.f;
	float y = 0.f;
};

constexpr int kRegisteredPoolTypes = 16;

template <int Base, std::size_t... Index>
void RegisterPoolTypes(EntityManager& entities, DEPRECATED_EntityId owner, std::index_sequence<Index...>)
{
	(entities.AddComponent<BenchData<Base + static_cast<int>(Index)>>(owner), ...);
}

template <typename T>
void DestroyAllWith(EntityManager& entities)
{
	entities.DestroyAllEntitiesWithComponent<T>();
}

std::vector<DEPRECATED_EntityId> CreateWith1(EntityManager& entities, std::int64_t count)
{
	std::vector<DEPRECATED_EntityId> ids;
	ids.reserve(static_cast<std::size_t>(count));
	for (std::int64_t i = 0; i < count; ++i)
	{
		const DEPRECATED_EntityId entity = entities.CreateEntity();
		entities.AddComponent<BenchData<1>>(entity, { static_cast<float>(i), 0.f });
		ids.push_back(entity);
	}
	return ids;
}
} // namespace

// CreateEntity + one AddComponent per entity (#10: Add allocates a node).
static void CreateEntities(benchmark::State& state)
{
	auto&      entities = EntityManager::Get();
	const auto count    = state.range(0);
	for (auto _ : state)
	{
		for (std::int64_t i = 0; i < count; ++i)
			entities.AddComponent<BenchData<0>>(entities.CreateEntity());

		state.PauseTiming();
		DestroyAllWith<BenchData<0>>(entities);
		state.ResumeTiming();
	}
	state.SetItemsProcessed(state.iterations() * count);
}

// Linear pass over one pool (#10: non-contiguous iteration).
static void Iterate1(benchmark::State& state)
{
	auto& entities = EntityManager::Get();
	CreateWith1(entities, state.range(0));
	auto* pool = entities.GetPool<BenchData<1>>();

	for (auto _ : state)
	{
		pool->ForEach([](DEPRECATED_EntityId, BenchData<1>& data) { data.x += 1.f; });
		benchmark::ClobberMemory();
	}
	state.SetItemsProcessed(state.iterations() * state.range(0));
	DestroyAllWith<BenchData<1>>(entities);
}

// Iterate pool A, look up pool B per entity: the BounceSystem pattern (#11).
static void Join2(benchmark::State& state)
{
	auto&      entities = EntityManager::Get();
	const auto count    = state.range(0);
	for (std::int64_t i = 0; i < count; ++i)
	{
		const DEPRECATED_EntityId entity = entities.CreateEntity();
		entities.AddComponent<BenchData<2>>(entity, { 1.f, 1.f });
		entities.AddComponent<BenchData<3>>(entity);
	}
	auto* velocities = entities.GetPool<BenchData<2>>();
	auto* positions  = entities.GetPool<BenchData<3>>();

	for (auto _ : state)
	{
		velocities->ForEach(
		    [&](DEPRECATED_EntityId entity, const BenchData<2>& velocity)
		    {
			    if (auto* position = positions->Get(entity))
			    {
				    position->x += velocity.x;
				    position->y += velocity.y;
			    }
		    });
		benchmark::ClobberMemory();
	}
	state.SetItemsProcessed(state.iterations() * count);
	DestroyAllWith<BenchData<2>>(entities);
}

// Iterate a large pool, look up an optional component only 10% of entities have (#11).
static void Join2Sparse(benchmark::State& state)
{
	auto&      entities = EntityManager::Get();
	const auto count    = state.range(0);
	for (std::int64_t i = 0; i < count; ++i)
	{
		const DEPRECATED_EntityId entity = entities.CreateEntity();
		entities.AddComponent<BenchData<4>>(entity);
		if (i % 10 == 0)
			entities.AddComponent<BenchData<5>>(entity, { 1.f, 0.f });
	}
	auto* all      = entities.GetPool<BenchData<4>>();
	auto* optional = entities.GetPool<BenchData<5>>();

	for (auto _ : state)
	{
		all->ForEach(
		    [&](DEPRECATED_EntityId entity, BenchData<4>& data)
		    {
			    if (const auto* extra = optional->Get(entity))
				    data.x += extra->x;
		    });
		benchmark::ClobberMemory();
	}
	state.SetItemsProcessed(state.iterations() * count);
	DestroyAllWith<BenchData<4>>(entities);
}

// Add then remove one component on every entity (#10: allocation per Add).
static void AddRemoveChurn(benchmark::State& state)
{
	auto&      entities = EntityManager::Get();
	const auto ids      = CreateWith1(entities, state.range(0));
	entities.AddComponent<BenchData<6>>(ids.front()); // make sure the pool exists
	auto* churnPool = entities.GetPool<BenchData<6>>();

	for (auto _ : state)
	{
		for (const DEPRECATED_EntityId entity : ids)
			churnPool->Add(entity, {});
		for (const DEPRECATED_EntityId entity : ids)
			churnPool->RemoveEntity(entity);
	}
	state.SetItemsProcessed(state.iterations() * state.range(0) * 2);
	DestroyAllWith<BenchData<1>>(entities);
}

// DestroyEntity with 16 registered component types (#14: virtual call on every pool).
static void DestroyEntities(benchmark::State& state)
{
	auto&          entities = EntityManager::Get();
	const DEPRECATED_EntityId owner    = entities.CreateEntity();
	RegisterPoolTypes<100>(entities, owner, std::make_index_sequence<kRegisteredPoolTypes>{});

	const auto count = state.range(0);
	for (auto _ : state)
	{
		state.PauseTiming();
		const auto ids = CreateWith1(entities, count);
		state.ResumeTiming();

		for (const DEPRECATED_EntityId entity : ids)
			entities.DestroyEntity(entity);
	}
	state.SetItemsProcessed(state.iterations() * count);
	entities.DestroyEntity(owner);
}

// Cost of GetPool<T>() alone, with 16 other types registered (#12: typeid + hash).
static void GetPool(benchmark::State& state)
{
	auto&          entities = EntityManager::Get();
	const DEPRECATED_EntityId owner    = entities.CreateEntity();
	RegisterPoolTypes<200>(entities, owner, std::make_index_sequence<kRegisteredPoolTypes>{});

	for (auto _ : state)
		benchmark::DoNotOptimize(entities.GetPool<BenchData<207>>());
	entities.DestroyEntity(owner);
}

BENCHMARK(CreateEntities)->Arg(1'000)->Arg(10'000)->Arg(100'000)->Unit(benchmark::kMicrosecond);
BENCHMARK(Iterate1)->Arg(1'000)->Arg(10'000)->Arg(100'000)->Unit(benchmark::kMicrosecond);
BENCHMARK(Join2)->Arg(1'000)->Arg(10'000)->Arg(100'000)->Unit(benchmark::kMicrosecond);
BENCHMARK(Join2Sparse)->Arg(1'000)->Arg(10'000)->Arg(100'000)->Unit(benchmark::kMicrosecond);
BENCHMARK(AddRemoveChurn)->Arg(1'000)->Arg(10'000)->Arg(100'000)->Unit(benchmark::kMicrosecond);
BENCHMARK(DestroyEntities)->Arg(1'000)->Arg(10'000)->Arg(100'000)->Unit(benchmark::kMicrosecond);
BENCHMARK(GetPool);
