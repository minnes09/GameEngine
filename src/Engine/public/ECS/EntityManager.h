#pragma once

#include "ECS/ComponentPool.h"
#include "ECS/DEPRECATED_EntityId.h"
#include "ECS/IComponentPool.h"

#include <cstdint>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

class EntityManager
{
public:
	using ComponentPoolMap = std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>>;

	// Single-instance engine service (Meyers singleton): the sole instance lives inside Get().
	// Non-copyable / non-movable so it can only ever exist once.
	EntityManager(const EntityManager&)            = delete;
	EntityManager& operator=(const EntityManager&) = delete;
	EntityManager(EntityManager&&)                 = delete;
	EntityManager& operator=(EntityManager&&)      = delete;

	static EntityManager& Get()
	{
		static EntityManager instance;
		return instance;
	}

	DEPRECATED_EntityId CreateEntity();
	DEPRECATED_EntityId CloneEntity(DEPRECATED_EntityId source);
	void DestroyEntity(DEPRECATED_EntityId entity);

	template <typename T>
	T& AddComponent(DEPRECATED_EntityId entity, T component = T{})
	{
		return getOrCreatePool<T>().Add(entity, std::move(component));
	}

	template <typename T>
	T* GetComponent(DEPRECATED_EntityId entity)
	{
		if (auto* pool = getPool<T>())
			return pool->Get(entity);
		return nullptr;
	}

	template <typename T>
	ComponentPool<T>* GetPool()
	{
		return getPool<T>();
	}

	template<class T>
	void DestroyAllEntitiesWithComponent()
	{
		const auto* pool = GetPool<T>();
		if (!pool)
			return;
		std::vector<DEPRECATED_EntityId> ids;
		pool->ForEach([&](DEPRECATED_EntityId id, const auto&) { ids.push_back(id); });
		for (DEPRECATED_EntityId id : ids)
			DestroyEntity(id);
	}

private:
	template <typename T>
	ComponentPool<T>* getPool()
	{
		auto it = componentPools.find(std::type_index(typeid(T)));
		if (it == componentPools.end())
			return nullptr;
		return static_cast<ComponentPool<T>*>(it->second.get());
	}

	template <typename T>
	ComponentPool<T>& getOrCreatePool()
	{
		auto& slot = componentPools[std::type_index(typeid(T))];
		if (!slot)
			slot = std::make_unique<ComponentPool<T>>();
		return *static_cast<ComponentPool<T>*>(slot.get());
	}

	EntityManager() = default;

	ComponentPoolMap componentPools;
	std::uint32_t    nextId = 0;
};
