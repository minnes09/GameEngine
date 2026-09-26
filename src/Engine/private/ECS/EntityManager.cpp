#include "ECS/EntityManager.h"

EntityId EntityManager::CreateEntity()
{
	return EntityId{ nextId++ };
}

void EntityManager::DestroyEntity(EntityId entity)
{
	for (auto& [_, pool] : componentPools)
		pool->RemoveEntity(entity);
}

EntityId EntityManager::CloneEntity(EntityId source)
{
	const EntityId clone = CreateEntity();
	for (auto& [_, pool] : componentPools)
		pool->CopyComponent(source, clone);
	return clone;
}
