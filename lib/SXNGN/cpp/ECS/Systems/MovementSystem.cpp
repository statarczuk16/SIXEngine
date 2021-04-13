
#include<Collision.h>
#include <ECS/Core/Coordinator.hpp>
#include <Database.h>
#include <ECS/Systems/MovementSystem.hpp>
#include <ECS/Utilities/ECS_Utils.hpp>

namespace SXNGN::ECS::A {

void Movement_System::Init()
{
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Movement_System Init");
}

//fixme
//readonly get of immoveables
//mutex check out of moveables
//will probably need to update recursivelly for moveables
//for now
void Movement_System::Update(float dt)
{

	auto gCoordinator = *SXNGN::Database::get_coordinator();
	std::vector<Entity> entities_to_cleanup;
	//std::vector<const Collisionable*> collisionables;
	std::vector<Entity> collisionable_entity_ids;
	//bool loaded_collisionables = false;
	//bool need_collisionables = false;

	//Iterate through entities this system manipulates/converts

	auto it_act = m_actable_entities.begin();
	//actable entities for user input system are UserInputCache (vector of sdl events)
	while (it_act != m_actable_entities.end())
	{
		auto const& entity_actable = *it_act;
		it_act++;

		//thread safe checkout of data
		auto check_out_move = gCoordinator.CheckOutComponent(entity_actable, ComponentTypeEnum::MOVEABLE);
		if (check_out_move)
		{


			//If no Collision, Update_Position_Without_Collision
			Moveable* moveable_ptr = static_cast<Moveable*>(check_out_move);
			//update position
			Update_Position(moveable_ptr, entity_actable, dt);
			//check data back in
			gCoordinator.CheckInComponent(ComponentTypeEnum::MOVEABLE, entity_actable);
		}

	}
}

void Movement_System::Update_Position(Moveable * moveable, Entity moveable_id, float dt)
{
	//SDL_Rect prev_pos = moveable->position_box_;

	auto gCoordinator = *SXNGN::Database::get_coordinator();

	switch (moveable->moveable_type_)
	{
	case  MoveableType::VELOCITY:
	{
		double new_x = moveable->m_pos_x_m + SXNGN::PIXELS_TO_METERS * (moveable->m_vel_x_m_s * dt);
		double new_y = moveable->m_pos_y_m + SXNGN::PIXELS_TO_METERS * (moveable->m_vel_y_m_s * dt);
		gCoordinator.CheckInComponent(ComponentTypeEnum::MOVEABLE, moveable_id);
		ECS_Utils::ChangeEntityPosition(moveable_id, new_x, new_y, false);
		gCoordinator.CheckOutComponent(moveable_id, ComponentTypeEnum::MOVEABLE);

	}
	case  MoveableType::FORCE:
	{

		//todo
	}
	}
}

}
