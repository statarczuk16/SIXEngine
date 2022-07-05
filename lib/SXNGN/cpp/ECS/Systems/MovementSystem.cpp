
#include<Collision.h>
#include <ECS/Core/Coordinator.hpp>
#include <Database.h>
#include <ECS/Systems/MovementSystem.hpp>
#include <ECS/Utilities/ECS_Utils.hpp>
#include <ECS/Utilities/Map_Utils.hpp>

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
		auto check_out_location = gCoordinator.CheckOutComponent(entity_actable, ComponentTypeEnum::LOCATION);
		if (check_out_move && check_out_location)
		{
			//If no Collision, Update_Position_Without_Collision
			Moveable* moveable_ptr = static_cast<Moveable*>(check_out_move);

			Location* location_ptr = static_cast<Location*>(check_out_move);
			//update position
			Update_Position(moveable_ptr, location_ptr, entity_actable, dt);
			//check data back in
			gCoordinator.CheckInComponent(ComponentTypeEnum::MOVEABLE, entity_actable);
			gCoordinator.CheckInComponent(ComponentTypeEnum::LOCATION, entity_actable);
		}

	}
}

void Movement_System::Translate_Waypoints_To_Movement(Location* location, Moveable* moveable)
{
	if (moveable)
	{
		switch (moveable->moveable_type_)
		{
		case MoveableType::VELOCITY:
		{
			
			auto destination = moveable->GetCurrentWaypoint();
			if (destination.x == -1 || destination.y == -1)
			{
				return;
			}
			auto position = location->GetGridCoordinate();
			if (position.x == -1 || position.y == -1)
			{
				return;
			}
			std::pair<double, double> movement_vector = Map_Utils::GetVector(position, destination);
			double vel_x = movement_vector.first * moveable->m_speed_m_s;
			double vel_y = movement_vector.second * moveable->m_speed_m_s;
			

			moveable->m_vel_x_m_s = (int)round(vel_x);
			moveable->m_vel_y_m_s = (int)round(vel_y);
			
			
		}
		break;
		case MoveableType::FORCE:
		{
			printf("Physics movement not yet supported");
		}
		break;
		default:
		{
			printf("Unsupported moveable type_ : %2d", moveable->moveable_type_);
		}

		}
	}
}

void Movement_System::Update_Position(Moveable * moveable, Location* location, Entity moveable_id, float dt)
{
	//First update moveable and linked components (collison box, renderable) to the potential position.
	//intended delta is set at the bottom of this function, and then verified by collision system, such that whatever intended_delta is present at the start of this function is considered confirmed delta.
	auto gCoordinator = *SXNGN::Database::get_coordinator();
	double confirmed_x = location->m_pos_x_m_ + moveable->m_intended_delta_x_m;
	double confirmed_y = location->m_pos_y_m_ + moveable->m_intended_delta_y_m;
	location->m_pos_x_m_ = confirmed_x;
	location->m_pos_y_m_ = confirmed_y;
	int grid_x = (int)round(confirmed_x);
	int grid_y = (int)round(confirmed_y);
	sole::uuid uuid = gCoordinator.GetUUIDFromEntity(moveable_id);
	gCoordinator.removeUUIDFromLocationMap(uuid, SXNGN::DEFAULT_SPACE);
	gCoordinator.addUUIDToLocationMap(grid_x, grid_y, uuid, SXNGN::DEFAULT_SPACE);
	gCoordinator.updateCollisionMap(uuid, SXNGN::DEFAULT_SPACE);

	moveable->m_intended_delta_x_m = 0;
	moveable->m_intended_delta_y_m = 0;

	if (moveable->Check_At_Destination(confirmed_x, confirmed_y))
	{
		moveable->m_vel_x_m_s = 0;
		moveable->m_vel_y_m_s = 0;
		return;
	}
	bool at_waypoint = moveable->Check_At_Waypoint(confirmed_x, confirmed_y);

	Translate_Waypoints_To_Movement(location, moveable); //sets the velocity vals of the moveable component

	switch (moveable->moveable_type_)
	{
		case  MoveableType::VELOCITY:
		{
			if (moveable->m_vel_x_m_s == 0.0 && moveable->m_vel_y_m_s == 0.0)
			{
				
			}
			else
			{
				moveable->m_intended_delta_x_m = SXNGN::PIXELS_TO_METERS * (moveable->m_vel_x_m_s * dt);
				moveable->m_intended_delta_y_m = SXNGN::PIXELS_TO_METERS * (moveable->m_vel_y_m_s * dt);
			}
					
		}
		case  MoveableType::FORCE:
		{

			//todo
		}
	}

	//adjust delta x or delta y to snap to waypoint is would have overshot it
	if (moveable->GetNumWaypoints() > 0)
	{
		double waypoint_snap_pos_x = location->m_pos_x_m_ + moveable->m_intended_delta_x_m;
		double waypoint_snap_pos_y = location->m_pos_y_m_ + moveable->m_intended_delta_y_m;
		double intended_pos_y = waypoint_snap_pos_y;
		double intended_pos_x = waypoint_snap_pos_x;
		double waypoint_snap_delta_x = std::abs(intended_pos_x - moveable->GetCurrentWaypoint().x);
		double waypoint_snap_delta_y = std::abs(intended_pos_y - moveable->GetCurrentWaypoint().y);
		
		if (moveable->m_vel_x_m_s >= 0 && intended_pos_x >= moveable->GetCurrentWaypoint().x)
		{
			moveable->m_intended_delta_x_m -= waypoint_snap_delta_x;
		}
		else if (moveable->m_vel_x_m_s < 0 && intended_pos_x < moveable->GetCurrentWaypoint().x)
		{
			moveable->m_intended_delta_x_m += waypoint_snap_delta_x;
		}

		if (moveable->m_vel_y_m_s >= 0 && intended_pos_y >= moveable->GetCurrentWaypoint().y)
		{
			moveable->m_intended_delta_y_m -= waypoint_snap_delta_y;
		}
		else if (moveable->m_vel_y_m_s < 0 && intended_pos_y < moveable->GetCurrentWaypoint().y)
		{
			moveable->m_intended_delta_y_m += waypoint_snap_delta_y;
		}
	}
}

}
