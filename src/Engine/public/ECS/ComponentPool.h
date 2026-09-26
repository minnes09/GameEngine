#pragma once

#include "ECS/EntityId.h"
#include "ECS/IComponentPool.h"

#include <unordered_map>
#include <utility>

template <typename T>
class ComponentPool : public IComponentPool
{
public:
	T& Add(EntityId entity, T component)
	{
		auto [it, _] = components.try_emplace(entity, std::move(component));
		return it->second;
	}

	T* Get(EntityId entity)
	{
		auto it = components.find(entity);
		return it != components.end() ? &it->second : nullptr;
	}

	const T* Get(EntityId entity) const
	{
		auto it = components.find(entity);
		return it != components.end() ? &it->second : nullptr;
	}

	void RemoveEntity(EntityId entity) override
	{
		components.erase(entity);
	}

	void CopyComponent(EntityId from, EntityId to) override
	{
		auto it = components.find(from);
		if (it == components.end())
			return;

		T copy = it->second;
		components[to] = std::move(copy);
	}

	template <typename Callback>
	void ForEach(Callback&& callback)
	{
		for (auto& [entity, component] : components)
			callback(entity, component);
	}

	template <typename Callback>
	void ForEach(Callback&& callback) const
	{
		for (const auto& [entity, component] : components)
			callback(entity, component);
	}

	auto begin()       { return components.begin(); }
	auto end()         { return components.end(); }
	auto begin() const { return components.begin(); }
	auto end()   const { return components.end(); }
	int size() const
	{
		return static_cast<int>(components.size());
	}

private:
	std::unordered_map<EntityId, T> components;
};
