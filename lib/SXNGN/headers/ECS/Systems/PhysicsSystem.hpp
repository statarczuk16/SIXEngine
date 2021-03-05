#pragma once

#include "ECS/Core/System.hpp"
#include <memory>
#include <SDL.h>
#include <ECS/Components/Moveable.hpp>
#include <ECS/Components/Collision.hpp>

using Moveable = SXNGN::ECS::Components::Moveable;
using MoveableType = SXNGN::ECS::Components::MoveableType;
using CollisionComponent = SXNGN::ECS::Components::CollisionComponent;

class Physics_System : public System
{
public:
	void Init();

	void Update(float dt);

	void Update_Position(Moveable* moveable, Entity moveable_id, std::vector<const CollisionComponent*>, std::vector<Entity>, float dt);

	std::pair< std::vector<const CollisionComponent*>, std::vector<Entity>> Load_Collision_Data(std::set<Entity> entities_with_collision);

private:


};