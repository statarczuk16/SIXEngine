#pragma once

#include "ECS/Core/System.hpp"
#include <memory>
#include <SDL.h>
#include <ECS/Components/Moveable.hpp>
#include <ECS/Components/Collision.hpp>
#include <ECS/Components/Renderable.hpp>

using Moveable = SXNGN::ECS::Components::Moveable;
using Renderable = SXNGN::ECS::Components::Renderable;
using MoveableType = SXNGN::ECS::Components::MoveableType;
//using Collisionable = SXNGN::ECS::Components::Collisionable;

class Movement_System : public System
{
public:
	void Init();

	void Update(float dt);

	void Update_Position(Moveable* moveable, Entity moveable_id, float dt);

	//std::pair< std::vector<const Collisionable*>, std::vector<Entity>> Load_Collision_Data(std::set<Entity> entities_with_collision);

	//void Update_Position_With_Collision(Moveable* moveable, Entity moveable_id, std::vector<const Collisionable*> collisionables, std::vector<Entity> collisionable_entity_ids, float dt);
private:


};
