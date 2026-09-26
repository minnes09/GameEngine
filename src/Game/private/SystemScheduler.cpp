#include "SystemScheduler.h"

#include "ECS/Components/Velocity.h"
#include "ECS/EntityManager.h"
#include "ECS/Systems/System.h"

#include <algorithm>
#include <cmath>
#include <utility>

namespace SystemScheduler_Private
{
// maxFrameTime clamps a catastrophic stall so the substep count can't explode — past it the sim
// runs in slow-motion rather than tunneling. maxSubSteps is a hard safety ceiling.
constexpr float maxFrameTime = 0.1f;
constexpr int   maxSubSteps  = 64;
} // namespace SystemScheduler_Private

SystemScheduler::SystemScheduler(std::vector<ISystem*> orderedSystems, float maxDistancePerStep)
    : systems(std::move(orderedSystems))
    , maxSubStepDistance(maxDistancePerStep)
{
}

void SystemScheduler::Step(float deltaTime)
{
	deltaTime = std::min(deltaTime, SystemScheduler_Private::maxFrameTime);

	// Size the substep count so the fastest mover travels at most maxSubStepDistance per step.
	// A discrete AABB pass only samples the current position, so without this an entity crossing more
	// than its own size in one frame skips the wall and leaves the field.
	const float maxTravel = maxMovementSpeed() * deltaTime;
	int subSteps = 1;
	if (maxTravel > maxSubStepDistance)
		subSteps = static_cast<int>(std::ceil(maxTravel / maxSubStepDistance));
	subSteps = std::min(subSteps, SystemScheduler_Private::maxSubSteps);

	// Every system updates each substep to avoid tunneling and keep the simulation coherent.
	// Order is defined by the caller (the list passed at construction).
	const float subDt = deltaTime / static_cast<float>(subSteps);
	for (int i = 0; i < subSteps; ++i)
		for (ISystem* system : systems)
			system->Update(subDt);
}

float SystemScheduler::maxMovementSpeed() const
{
	auto* movementPool = EntityManager::Get().GetPool<MovementComponent>();
	if (!movementPool)
		return 0.f;

	float maxSpeedSq = 0.f;
	movementPool->ForEach(
	    [&](EntityId, const MovementComponent& movement)
	    {
		    const float speedSq = movement.velocity.x * movement.velocity.x
		                        + movement.velocity.y * movement.velocity.y;
		    maxSpeedSq          = std::max(maxSpeedSq, speedSq);
	    });
	return std::sqrt(maxSpeedSq);
}
