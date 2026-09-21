#pragma once

#include <vector>

class ISystem;

// Engine-generic fixed-order stepper. Drives a caller-supplied list of systems, subdividing the
// frame into substeps so the fastest mover never travels more than maxSubStepDistance per step
// (discrete AABB collision only samples the current position, so a large single step tunnels).
class SystemScheduler
{
public:
	SystemScheduler(std::vector<ISystem*> systems, float maxSubStepDistance);

	void Step(float deltaTime);

private:
	// Largest velocity magnitude across MovementComponents this frame; drives the substep count.
	float maxMovementSpeed() const;

	std::vector<ISystem*> m_systems;
	float                 m_maxSubStepDistance;
};
