#include "ECS/Systems/PhysicsSystem.hpp"

#include "ECS/Components/Gravity.hpp"
#include "ECS/Components/RigidBody.hpp"
#include "ECS/Components/Thrust.hpp"
#include "ECS/Components/Transform.hpp"
#include "ECS/Core/Coordinator.hpp"


extern Coordinator gCoordinator;


void PhysicsSystem::Init()
{
}

void PhysicsSystem::Update(float dt)
{
	for (auto const& entity : mEntities)
	{
		auto& rigidBody = gCoordinator.GetComponent<RigidBody>(entity);
		auto& transform = gCoordinator.GetComponent<Transform>(entity);

		// Forces
		auto const& gravity = gCoordinator.GetComponent<Gravity>(entity);

		transform.position += rigidBody.velocity * dt;

		rigidBody.velocity += gravity.force * dt;
	}
}
