#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>



using ComponentTypeEnum = SXNGN::ECS::ComponentTypeEnum;

namespace SXNGN::ECS {


	/// <summary>
	/// Contains data about a ground level tile
	/// </summary>
	struct Tile : ECS_Component
	{


		Tile(int traversal_cost = 1)
		{
			component_type = ComponentTypeEnum::TILE;
			traversal_cost_ = traversal_cost;
		}
		int traversal_cost_ = 0;

	};


}