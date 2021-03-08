#include <ECS/Systems/PhysicsSystem.hpp>
#include<Collision.h>
#include <ECS/Core/Coordinator.hpp>
#include <ECS/Components/Collision.hpp>
#include <Collision.h>
#include <Database.h>

using CollisionComponent = SXNGN::ECS::Components::CollisionComponent;
void Physics_System::Init()
{
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Renderer_System Init");

}

//fixme
//readonly get of immoveables
//mutex check out of moveables
//will probably need to update recursivelly for moveables
//for now
void Physics_System::Update(float dt)
{

	auto gCoordinator = *SXNGN::Database::get_coordinator();
	std::vector<Entity> entities_to_cleanup;
	std::vector<const CollisionComponent*> collisionables;
	std::vector<Entity> collisionable_entity_ids;
	bool loaded_collisionables = false;
	bool need_collisionables = false;

	auto it_intr = m_entities_of_interest.begin();
	while (it_intr != m_entities_of_interest.end())
	{
		auto const& entity_interest = *it_intr;
		it_intr++;
		auto collision_comp = gCoordinator.GetComponentReadOnly(entity_interest, ComponentTypeEnum::COLLISION);
		const CollisionComponent* collision_ptr = static_cast<const CollisionComponent*>(collision_comp);
		collisionables.push_back(collision_ptr);
		collisionable_entity_ids.push_back(entity_interest);//keep track of entity id of each collisionable to prevent an object from colliding with itself
	}

	//Iterate through entities this system manipulates/converts

	auto it_act = m_actable_entities.begin();
	//actable entities for user input system are UserInputCache (vector of sdl events)
	while (it_act != m_actable_entities.end())
	{
		auto const& entity_actable = *it_act;
		it_act++;

		//thread safe checkout of data
		auto check_out_move = gCoordinator.CheckOutComponent(entity_actable, ComponentTypeEnum::MOVEABLE);
		auto check_out_collision = gCoordinator.CheckOutComponent(entity_actable, ComponentTypeEnum::COLLISION);
		if (check_out_move.first)
		{
			//Check if Moveable also has Collisions
			//If so, call Update_Position_With_Collision -> Read in all Collision data if have not done so yet (Read only, not checkout)
			//Update currenty entity's collision data in the read-only vector
			if (check_out_collision.first)
			{

				if (!loaded_collisionables)
				{
					auto collisionables_to_entity_ids = Load_Collision_Data(m_entities_of_interest);
					loaded_collisionables = true;


				}
			}

			//If no Collision, Update_Position_Without_Collision
			Moveable* moveable_ptr = static_cast<Moveable*>(check_out_move.first);
			//update position
			Update_Position_With_Collision(moveable_ptr, entity_actable,collisionables,collisionable_entity_ids, dt);
			//check data back in
			gCoordinator.CheckInComponent(ComponentTypeEnum::MOVEABLE, entity_actable, std::move(check_out_move.second));
			gCoordinator.CheckInComponent(ComponentTypeEnum::COLLISION, entity_actable, std::move(check_out_collision.second));
		}

	}
}

std::pair< std::vector<const CollisionComponent*>, std::vector<Entity>> Physics_System::Load_Collision_Data(std::set<Entity> entities_with_collision)
{
	auto gCoordinator = *SXNGN::Database::get_coordinator();
	std::vector<const CollisionComponent*> collisionables;
	std::vector<Entity> collisionable_entity_ids;
	auto it_intr = entities_with_collision.begin();
	while (it_intr != entities_with_collision.end())
	{
		auto const& entity_interest = *it_intr;
		it_intr++;
		auto collision_comp = gCoordinator.GetComponentReadOnly(entity_interest, ComponentTypeEnum::COLLISION);
		const CollisionComponent* collision_ptr = static_cast<const CollisionComponent*>(collision_comp);
		collisionables.push_back(collision_ptr);
		collisionable_entity_ids.push_back(entity_interest);//keep track of entity id of each collisionable to prevent an object from colliding with itself
	}
	return std::make_pair ( collisionables, collisionable_entity_ids);
}


void Physics_System::Update_Position_With_Collision(Moveable* moveable, Entity moveable_id, std::vector<const CollisionComponent*> collisionables, std::vector<Entity> collisionable_entity_ids, float dt)
{
	SDL_Rect prev_pos = moveable->position_box_;
	auto gCoordinator = *SXNGN::Database::get_coordinator();
	auto this_moveable_collision = gCoordinator;
	std::vector<std::vector<const SXNGN::ECS::Components::CollisionComponent*>> collisions;
	switch (moveable->moveable_type_)
	{
		case  MoveableType::VELOCITY:
		{

			moveable->position_box_.x += int(moveable->m_vel_x_m_s * dt);
			moveable->position_box_.y += int(moveable->m_vel_y_m_s * dt);
			/**collisions = SXNGN::CollisionChecks::determineCollisions(moveable->position_box_, collisionables, 0);
			if (collisions[0].empty() && collisions[1].empty())
			{

			}**/
		}
	}
}
