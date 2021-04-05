
#include<Collision.h>
#include <ECS/Core/Coordinator.hpp>
#include <Database.h>
#include <ECS/Systems/MovementSystem.hpp>

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

		moveable->m_prev_pos_x_m = moveable->m_pos_x_m;
		moveable->m_prev_pos_y_m = moveable->m_pos_y_m;
		moveable->m_pos_x_m += SXNGN::PIXELS_TO_METERS * (moveable->m_vel_x_m_s * dt);
		moveable->m_pos_y_m += SXNGN::PIXELS_TO_METERS * (moveable->m_vel_y_m_s * dt);

		if (moveable->m_vel_x_m_s > 0)
		{
			//printf("Renderable Entity %d: Velocity (%d,%d)\n", moveable_id, moveable->m_vel_x_m_s, moveable->m_vel_y_m_s);
			//printf("Renderable Entity %d: Pos(%g,%g) -> (%g,%g)\n", moveable_id, moveable->m_pos_x_m, moveable->m_pos_y_m, moveable->m_prev_pos_x_m, moveable->m_prev_pos_y_m);
		}

		if (gCoordinator.EntityHasComponent(moveable_id, ComponentTypeEnum::RENDERABLE))
		{
			auto moveable_renderbox = gCoordinator.CheckOutComponent(moveable_id, ComponentTypeEnum::RENDERABLE);
			if (moveable_renderbox)
			{
				Renderable* render_ptr = static_cast<Renderable*>(moveable_renderbox);
				render_ptr->x_ = int(round(moveable->m_pos_x_m));
				render_ptr->y_ = int(round(moveable->m_pos_y_m));
				if (moveable->m_pos_x_m != moveable->m_prev_pos_x_m || moveable->m_pos_y_m != moveable->m_prev_pos_y_m)
				{
					//printf("Renderable Entity: %2d Position: (%2d,%2d)\n", moveable_id, moveable->m_pos_x_m, moveable->m_prev_pos_x_m);
				}
				gCoordinator.CheckInComponent(ComponentTypeEnum::RENDERABLE, moveable_id);
			}
		}

		if (gCoordinator.EntityHasComponent(moveable_id, ComponentTypeEnum::COLLISION))
		{
			auto collision_box = gCoordinator.CheckOutComponent(moveable_id, ComponentTypeEnum::COLLISION);
			if (collision_box)
			{
				Collisionable* collision_box_ptr = static_cast<Collisionable*>(collision_box);
				collision_box_ptr->collision_box_.x = int(round(moveable->m_pos_x_m));
				collision_box_ptr->collision_box_.y = int(round(moveable->m_pos_y_m));
				collision_box_ptr->resolved_ = false;
				
				gCoordinator.CheckInComponent(ComponentTypeEnum::COLLISION, moveable_id);
			}
		}

	}
	case  MoveableType::FORCE:
	{

		//todo
	}
	}
}

}
