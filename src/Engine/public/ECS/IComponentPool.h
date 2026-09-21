#pragma once

#include "ECS/EntityId.h"

class IComponentPool
{
public:
	virtual ~IComponentPool() = default;

	virtual void RemoveEntity(EntityId entity) = 0;
	virtual void CopyComponent(EntityId from, EntityId to) = 0;
};
