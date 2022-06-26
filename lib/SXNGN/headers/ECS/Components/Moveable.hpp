#pragma once

#include "SDL.h"
#include <Sprite/Tile.h>
#include <memory>
#include <ECS/Core/Component.hpp>
#include <ECS/Core/Types.hpp>
#include <unordered_map>
#include <queue>

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
		private:
			
		//SDL_Rect position_box_;
		public:
			double m_intended_delta_x_m = 0;//the potential position - will be taken up if not corrected by collision system this frame
			double m_intended_delta_y_m = 0;
			double m_vel_x_m_s = 0;//current speed
			double m_vel_y_m_s = 0;
			Sint32 m_speed_m_s = 0;//velocity added/removed per event
			MoveableType moveable_type_ = MoveableType::VELOCITY;
			NAVIGATION_TYPE navigation_type_ = NAVIGATION_TYPE::A_STAR;

			Coordinate destination_;
			bool new_destination_ = false;
			bool at_destination_ = false;
			std::queue<Coordinate> waypoints_;

			Moveable();

			Moveable(int speed_m_s);

			void Update_Destination(Coordinate new_location);
		
			bool Check_At_Destination(double x, double y);
		
			bool Check_At_Waypoint(double x, double y);

			bool SolveDestination(double x, double y, NAVIGATION_TYPE method);
		
			Coordinate GetCurrentWaypoint();

			unsigned int GetNumWaypoints();
		

		
	};

	//todo enum to string for collision_tag
	inline void to_json(json& j, const Moveable& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::MOVEABLE]},

			{"m_intended_delta_x_m", p.m_intended_delta_x_m},
			{"m_intended_delta_y_m", p.m_intended_delta_y_m},
			{"m_vel_x_m_s", p.m_vel_x_m_s},
			{"m_vel_y_m_s", p.m_vel_y_m_s},
			{"m_speed_m_s", p.m_speed_m_s},
			{"moveable_type_", p.moveable_type_},
			{"navigation_type_", p.navigation_type_},
			{"destination_", p.destination_},

			{"new_destination_", p.new_destination_},
			{"at_destination_", p.at_destination_},
			//{"waypoints_", p.waypoints_},
		};
	}

	inline void from_json(const json& j, Moveable& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		double m_pos_x;
		double m_pos_y;
		p.component_type = component_type_enum;


		j.at("m_intended_delta_x_m").get_to(p.m_intended_delta_x_m);
		j.at("m_intended_delta_y_m").get_to(p.m_intended_delta_y_m);
		j.at("m_vel_x_m_s").get_to(p.m_vel_x_m_s);
		j.at("m_vel_y_m_s").get_to(p.m_vel_y_m_s);
		j.at("m_speed_m_s").get_to(p.m_speed_m_s);
		j.at("moveable_type_").get_to(p.moveable_type_);
		j.at("navigation_type_").get_to(p.navigation_type_);
		j.at("destination_").get_to(p.destination_);

		j.at("new_destination_").get_to(p.new_destination_);
		j.at("at_destination_").get_to(p.at_destination_);
		//j.at("waypoints_").get_to(p.waypoints_);
		
	}
}
