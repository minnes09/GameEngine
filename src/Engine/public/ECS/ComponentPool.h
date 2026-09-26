#pragma once

#include "ECS/DEPRECATED_EntityId.h"
#include "ECS/IComponentPool.h"

#include <unordered_map>
#include <utility>

template <typename T>
class ComponentPool : public IComponentPool
{
public:
	T& Add(DEPRECATED_EntityId entity, T component)
	{
		auto [it, _] = components.try_emplace(entity, std::move(component));
		return it->second;
	}

	T* Get(DEPRECATED_EntityId entity)
	{
		auto it = components.find(entity);
		return it != components.end() ? &it->second : nullptr;
	}

	const T* Get(DEPRECATED_EntityId entity) const
	{
		auto it = components.find(entity);
		return it != components.end() ? &it->second : nullptr;
	}

	void RemoveEntity(DEPRECATED_EntityId entity) override
	{
		components.erase(entity);
	}

	void CopyComponent(DEPRECATED_EntityId from, DEPRECATED_EntityId to) override
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
	std::unordered_map<DEPRECATED_EntityId, T> components;
};
