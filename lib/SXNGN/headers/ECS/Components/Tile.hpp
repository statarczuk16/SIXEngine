#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>


using ComponentTypeEnum = SXNGN::ECS::Components::ComponentTypeEnum;

namespace SXNGN::ECS::Components {


	/// <summary>
	/// Contains data about a ground level tile
	/// </summary>
	struct Tile : ECS_Component
	{
		Tile(Sint32 grid_x, Sint32 grid_y)
		{
			grid_y_ = grid_y;
			grid_x_ = grid_x;
			component_type = ComponentTypeEnum::TILE;

		}
		Sint32  grid_x_;
		Sint32  grid_y_;
	};


}