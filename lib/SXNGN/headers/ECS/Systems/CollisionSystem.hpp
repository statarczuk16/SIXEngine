#pragma once

#include "ECS/Core/System.hpp"
#include <memory>
#include <SDL.h>
#include <ECS/Components/Moveable.hpp>
#include <ECS/Components/Collision.hpp>
#include <ECS/Components/Renderable.hpp>

namespace SXNGN::ECS::A {
	//using Collisionable = SXNGN::ECS::A::Collisionable;

	class Collision_System : public System
	{
	public:
		void Init();

		void Update(float dt);

		void Update_Position(Moveable* moveable, Entity moveable_id, float dt);

		int HandleCollisionEvent(Collisionable* event, Entity event_entity, Collisionable* other, Entity other_entity, Coordinator gCoordinator);

		int HandleCollisionPersonPerson(Collisionable* person, Entity person_entity, Collisionable* other, Entity other_entity, Coordinator gCoordinator);

		int HandleCollisionPersonProjectile(Collisionable* person, Entity person_entity, Collisionable* other, Entity other_entity, Coordinator gCoordinator);

		int HandleCollisionPersonTile(Collisionable* person, Entity person_entity, Collisionable* other, Entity other_entity, Coordinator gCoordinator);

		int HandleCollisionPersonObject(Collisionable* person, Entity person_entity, Collisionable* other, Entity other_entity, Coordinator gCoordinator);

		int HandleCollisionGeneric(Collisionable* person, Entity person_entity, Collisionable* other, Entity other_entity, Coordinator gCoordinator);


		//std::pair< std::vector<const Collisionable*>, std::vector<Entity>> Load_Collision_Data(std::set<Entity> entities_with_collision);

		//void Update_Position_With_Collision(Moveable* moveable, Entity moveable_id, std::vector<const Collisionable*> collisionables, std::vector<Entity> collisionable_entity_ids, float dt);
	private:


	};
}