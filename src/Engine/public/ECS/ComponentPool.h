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
		auto [it, _] = m_components.try_emplace(entity, std::move(component));
		return it->second;
	}

	T* Get(EntityId entity)
	{
		auto it = m_components.find(entity);
		return it != m_components.end() ? &it->second : nullptr;
	}

	const T* Get(EntityId entity) const
	{
		auto it = m_components.find(entity);
		return it != m_components.end() ? &it->second : nullptr;
	}

	void RemoveEntity(EntityId entity) override
	{
		m_components.erase(entity);
	}

	void CopyComponent(EntityId from, EntityId to) override
	{
		auto it = m_components.find(from);
		if (it == m_components.end())
			return;

		T copy = it->second;
		m_components[to] = std::move(copy);
	}

	template <typename Callback>
	void ForEach(Callback&& callback)
	{
		for (auto& [entity, component] : m_components)
			callback(entity, component);
	}

	template <typename Callback>
	void ForEach(Callback&& callback) const
	{
		for (const auto& [entity, component] : m_components)
			callback(entity, component);
	}

	auto begin()       { return m_components.begin(); }
	auto end()         { return m_components.end(); }
	auto begin() const { return m_components.begin(); }
	auto end()   const { return m_components.end(); }
	int size() const
	{
		return static_cast<int>(m_components.size());
	}

private:
	std::unordered_map<EntityId, T> m_components;
};
