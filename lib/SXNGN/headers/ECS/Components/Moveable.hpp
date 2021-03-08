#pragma once

#include "SDL.h"
#include <Sprite/Tile.h>
#include <memory>
#include <ECS/Core/Component.hpp>
#include <unordered_map>

//using ComponentTypeEnum = SXNGN::ECS::Components::ComponentTypeEnum;

namespace SXNGN::ECS::Components {


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
		Moveable()
		{
			component_type = ComponentTypeEnum::MOVEABLE;
			//position_box_.x = 0;
			//position_box_.y = 0;
			//position_box_.w = 1;
			//position_box_.h = 1;


		}
		//SDL_Rect position_box_;
		double m_pos_x_m = 0;//current speed
		double m_pos_y_m = 0;//current speed
		double m_prev_pos_x_m = 0;//current speed
		double m_prev_pos_y_m = 0;//current speed
		Sint32 m_vel_x_m_s = 0;//current speed
		Sint32 m_vel_y_m_s = 0;
		Sint32 m_speed_m_s = 0;//velocity added/removed per event
		MoveableType moveable_type_ = MoveableType::VELOCITY;
	};
}