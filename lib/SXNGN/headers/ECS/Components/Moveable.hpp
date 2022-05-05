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
		//SDL_Rect position_box_;
		double m_pos_x_m = 0;//current speed
		double m_pos_y_m = 0;//current speed
		double m_prev_pos_x_m = 0;//current speed
		double m_prev_pos_y_m = 0;//current speed
		Sint32 m_vel_x_m_s = 0;//current speed
		Sint32 m_vel_y_m_s = 0;
		Sint32 m_speed_m_s = 0;//velocity added/removed per event
		MoveableType moveable_type_ = MoveableType::VELOCITY;
		NAVIGATION_TYPE navigation_type_ = NAVIGATION_TYPE::A_STAR;

		Location destination_;
		Location position_;
		bool new_destination_ = false;
		bool at_destination_ = false;
		std::queue<Location> waypoints_;

		Moveable();

		void Update_Destination(Location new_location);
		

		bool Check_At_Destination();
		

		bool Check_At_Waypoint();
		

		bool SolveDestination(NAVIGATION_TYPE method);
		

		void UpdatePosition(double new_x, double new_y);
		

		Location GetCurrentWaypoint();
		

		Location GetPosition();
		
	};

	//todo enum to string for collision_tag
	inline void to_json(json& j, const Moveable& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::MOVEABLE]},
			{"m_pos_x_m", p.m_pos_x_m},
			{"m_pos_y_m", p.m_pos_y_m},
			{"m_prev_pos_x_m", p.m_prev_pos_x_m},
			{"m_prev_pos_y_m", p.m_prev_pos_y_m},
			{"m_vel_x_m_s", p.m_vel_x_m_s},
			{"m_vel_y_m_s", p.m_vel_y_m_s},
			{"m_speed_m_s", p.m_speed_m_s},
			{"moveable_type_", p.moveable_type_},
			{"navigation_type_", p.navigation_type_},
			{"destination_", p.destination_},
			{"position_", p.position_},
			{"new_destination_", p.new_destination_},
			{"at_destination_", p.at_destination_},
			//{"waypoints_", p.waypoints_},
		};
	}

	inline void from_json(const json& j, Moveable& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		j.at("m_pos_x_m").get_to(p.m_pos_x_m);
		j.at("m_pos_y_m").get_to(p.m_pos_y_m);
		j.at("m_prev_pos_x_m").get_to(p.m_prev_pos_x_m);
		j.at("m_prev_pos_y_m").get_to(p.m_prev_pos_y_m);
		j.at("m_vel_x_m_s").get_to(p.m_vel_x_m_s);
		j.at("m_vel_y_m_s").get_to(p.m_vel_y_m_s);
		j.at("m_speed_m_s").get_to(p.m_speed_m_s);
		j.at("moveable_type_").get_to(p.moveable_type_);
		j.at("navigation_type_").get_to(p.navigation_type_);
		j.at("destination_").get_to(p.destination_);
		j.at("position_").get_to(p.position_);
		j.at("new_destination_").get_to(p.new_destination_);
		j.at("at_destination_").get_to(p.at_destination_);
		//j.at("waypoints_").get_to(p.waypoints_);
		
	}
}
