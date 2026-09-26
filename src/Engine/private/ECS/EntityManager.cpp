#include "ECS/EntityManager.h"

DEPRECATED_EntityId EntityManager::CreateEntity()
{
	return DEPRECATED_EntityId{ nextId++ };
}

void EntityManager::DestroyEntity(DEPRECATED_EntityId entity)
{
	for (auto& [_, pool] : componentPools)
		pool->RemoveEntity(entity);
}

DEPRECATED_EntityId EntityManager::CloneEntity(DEPRECATED_EntityId source)
{
	const DEPRECATED_EntityId clone = CreateEntity();
	for (auto& [_, pool] : componentPools)
		pool->CopyComponent(source, clone);
	return clone;
}
