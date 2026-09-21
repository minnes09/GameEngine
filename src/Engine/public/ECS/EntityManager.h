#pragma once

#include "ECS/ComponentPool.h"
#include "ECS/EntityId.h"
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

	EntityId CreateEntity();
	EntityId CloneEntity(EntityId source);
	void DestroyEntity(EntityId entity);

	template <typename T>
	T& AddComponent(EntityId entity, T component = T{})
	{
		return getOrCreatePool<T>().Add(entity, std::move(component));
	}

	template <typename T>
	T* GetComponent(EntityId entity)
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
		std::vector<EntityId> ids;
		pool->ForEach([&](EntityId id, const auto&) { ids.push_back(id); });
		for (EntityId id : ids)
			DestroyEntity(id);
	}

private:
	template <typename T>
	ComponentPool<T>* getPool()
	{
		auto it = m_componentPools.find(std::type_index(typeid(T)));
		if (it == m_componentPools.end())
			return nullptr;
		return static_cast<ComponentPool<T>*>(it->second.get());
	}

	template <typename T>
	ComponentPool<T>& getOrCreatePool()
	{
		auto& slot = m_componentPools[std::type_index(typeid(T))];
		if (!slot)
			slot = std::make_unique<ComponentPool<T>>();
		return *static_cast<ComponentPool<T>*>(slot.get());
	}

	EntityManager() = default;

	ComponentPoolMap m_componentPools;
	std::uint32_t    m_nextId = 0;
};
