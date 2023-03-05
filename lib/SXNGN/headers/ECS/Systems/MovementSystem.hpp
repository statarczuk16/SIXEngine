#pragma once

#include <ECS/Core/System.hpp>
#include <memory>
#include <SDL.h>
#include <ECS/Components/Components.hpp>

namespace SXNGN::ECS {

	class Movement_System : public System
	{
	public:
		virtual void Init();

		void Update(double dt);

		void Update_Position(Moveable* moveable, Location* location, Entity moveable_id, double dt);

		void Translate_Waypoints_To_Movement(Location* location, Moveable* moveable);

		//std::pair< std::vector<const Collisionable*>, std::vector<Entity>> Load_Collision_Data(std::set<Entity> entities_with_collision);

		//void Update_Position_With_Collision(Moveable* moveable, Entity moveable_id, std::vector<const Collisionable*> collisionables, std::vector<Entity> collisionable_entity_ids, float dt);
	private:


	};
}