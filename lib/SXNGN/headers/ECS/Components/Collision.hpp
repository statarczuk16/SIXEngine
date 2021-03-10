#pragma once

#include "SDL.h"
#include <ECS/Core/Component.hpp>


namespace SXNGN::ECS::Components {


	enum class CollisionType
	{
		IMMOVEABLE,
		ELASTIC,
		NONE
	};


	/// <summary>
	/// Contains all data needed to handle a collision between two entities that both have collision
	/// </summary>
	struct Collisionable : ECS_Component
	{
		Collisionable()
		{
			component_type = ComponentTypeEnum::COLLISION;
			collision_type_ = CollisionType::IMMOVEABLE;
			collision_box_.x = 0;
			collision_box_.y = 0;
			collision_box_.w = 0;
			collision_box_.h = 0;
		}
		SDL_Rect collision_box_;
		CollisionType collision_type_ = CollisionType::IMMOVEABLE;
	};
}