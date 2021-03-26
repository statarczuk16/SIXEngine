#pragma once

#include "ECS/Core/System.hpp"
#include <memory>
#include <SDL.h>
#include <ECS/Components/Moveable.hpp>
#include <ECS/Components/Collision.hpp>
#include <ECS/Components/Renderable.hpp>

namespace SXNGN::ECS::A {
	using Moveable = A::Moveable;
	using Renderable = A::Renderable;
	using MoveableType = A::MoveableType;
	//using Collisionable = SXNGN::ECS::A::Collisionable;

	class Collision_System : public System
	{
	public:
		void Init();

		void Update(float dt);

		void Update_Position(Moveable* moveable, Entity moveable_id, float dt);

		//std::pair< std::vector<const Collisionable*>, std::vector<Entity>> Load_Collision_Data(std::set<Entity> entities_with_collision);

		//void Update_Position_With_Collision(Moveable* moveable, Entity moveable_id, std::vector<const Collisionable*> collisionables, std::vector<Entity> collisionable_entity_ids, float dt);
	private:


	};
}