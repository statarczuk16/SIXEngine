#pragma once

#include "ECS/Core/System.hpp"
#include <memory>
#include <SDL.h>
#include <ECS/Components/Moveable.hpp>
#include <ECS/Components/Collision.hpp>
#include <ECS/Components/Renderable.hpp>

namespace SXNGN::ECS {
	//using Collisionable = SXNGN::ECS::Collisionable;

	class Collision_System : public System
	{
	public:
		virtual void Init();

		void Update(double dt);

		void Update_Position(Moveable* moveable, Entity moveable_id, double dt);

		std::set<Entity> HandleCollisionEvent(Entity entity1, Collisionable* collisonable1_ptr, Entity entity2, Collisionable* collisonable2_ptr);

		/**int HandleCollisionPersonPerson(Collisionable* person, Entity person_entity, Collisionable* other, Entity other_entity, Coordinator gCoordinator);

		int HandleCollisionPersonProjectile(Collisionable* person, Entity person_entity, Collisionable* other, Entity other_entity, Coordinator gCoordinator);

		int HandleCollisionPersonTile(Collisionable* person, Entity person_entity, Collisionable* other, Entity other_entity, Coordinator gCoordinator);

		int HandleCollisionPersonObject(Collisionable* person, Entity person_entity, Collisionable* other, Entity other_entity, Coordinator gCoordinator);**/

		std::set<Entity> HandleCollisionDynamicOnStatic(Entity entity1, Collisionable* collisonable1_ptr, Entity entity2, Collisionable* collisonable2_ptr);
		//std::pair< std::vector<const Collisionable*>, std::vector<Entity>> Load_Collision_Data(std::set<Entity> entities_with_collision);

	private:


	};
}