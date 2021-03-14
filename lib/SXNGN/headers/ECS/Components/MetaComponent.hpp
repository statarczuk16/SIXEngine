#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>


using ComponentTypeEnum = SXNGN::ECS::A::ComponentTypeEnum;

namespace SXNGN::ECS::A {

	
	/// <summary>
	/// Contains data about a ground level tile
	/// </summary>
	struct MetaComponent : ECS_Component
	{
		MetaComponent()
		{
			
		}
		bool saveable_ = true; //this entity can be saved to disc
		bool all_states_ = false;//this entity should migrate to all states

	};


}