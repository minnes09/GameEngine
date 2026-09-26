#pragma once

#include "ECS/DEPRECATED_EntityId.h"

class IComponentPool
{
public:
	virtual ~IComponentPool() = default;

	virtual void RemoveEntity(DEPRECATED_EntityId entity) = 0;
	virtual void CopyComponent(DEPRECATED_EntityId from, DEPRECATED_EntityId to) = 0;
};
