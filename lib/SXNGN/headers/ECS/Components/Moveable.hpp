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
		}
		SDL_Rect position_box_;
		size_t m_vel_x_m_s = 0;//current speed
		size_t m_vel_y_m_s = 0;
		size_t m_speed_m_s = 0;//velocity added/removed per event
		MoveableType moveable_type_ = MoveableType::VELOCITY;
	};
}