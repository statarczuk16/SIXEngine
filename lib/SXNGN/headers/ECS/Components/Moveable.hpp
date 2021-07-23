#pragma once

#include "SDL.h"
#include <Sprite/Tile.h>
#include <memory>
#include <ECS/Core/Component.hpp>

#include <unordered_map>

//using ComponentTypeEnum = SXNGN::ECS::A::ComponentTypeEnum;

namespace SXNGN::ECS::A {


	enum class MoveableType
	{
		VELOCITY,
		FORCE
	};


	/// <summary>
	/// Contains all data needed to render a BASE_TILE_WIDTH*BASE_TILE_HEIGHT sprite
	/// </summary>
	struct Moveable : ECS_Component
	{
		//SDL_Rect position_box_;
		double m_pos_x_m = 0;//current speed
		double m_pos_y_m = 0;//current speed
		double m_prev_pos_x_m = 0;//current speed
		double m_prev_pos_y_m = 0;//current speed
		Sint32 m_vel_x_m_s = 0;//current speed
		Sint32 m_vel_y_m_s = 0;
		Sint32 m_speed_m_s = 0;//velocity added/removed per event
		MoveableType moveable_type_ = MoveableType::VELOCITY;
		NAVIGATION_TYPE navigation_type = NAVIGATION_TYPE::MANHATTAN;

		Location destination_;
		Location position_;
		bool new_destination_ = false;
		bool at_destination = false;
		std::queue<Location> waypoints;

			Moveable();

			void Update_Destination(Location new_location);
		

			bool Check_At_Destination();
		

			bool Check_At_Waypoint();
		

			void SolveDestination(NAVIGATION_TYPE method);
		

			void UpdatePosition(double new_x, double new_y);
		

			Location GetCurrentWaypoint();
		

			Location GetPosition();
		
	};
}
