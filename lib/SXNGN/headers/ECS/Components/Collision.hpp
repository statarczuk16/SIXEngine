#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>
#include <nlohmann/json.hpp>
using nlohmann::json;

namespace SXNGN::ECS::A {


	//dont fuck with this order will break saves fixme 
	enum class CollisionType
	{
		UNKNOWN,
		STATIC,
		DYNAMIC,
		NONE
	};

	enum class CollisionTag
	{
		UNKNOWN,
		EVENT,
		PERSON,
		PROJECTILE,
		TILE,
		OBJECT
	};

	enum class CollisionShape
	{
		UNKNOWN,
		RECTANGLE,
		CIRCLE
	};

	


	/// <summary>
	/// Contains all data needed to handle a collision between two entities that both have collision
	/// </summary>
	struct Collisionable : ECS_Component
	{
		Collisionable()
		{
			component_type = ComponentTypeEnum::COLLISION;
			collision_type_ = CollisionType::STATIC;
			buffer_pixels = 0;
			resolved_ = false;
			radius_ = 0;
			width_ = 0;
			height_ = 0;

		}

		Collisionable(Uint32 radius, CollisionType type = CollisionType::STATIC)
		{
			component_type = ComponentTypeEnum::COLLISION;
			radius_ = radius;
			buffer_pixels = 0;
			radius_ = 0;
			resolved_ = false;

			collision_type_ = type;
			if (type == CollisionType::STATIC)
			{
				resolved_ = true;
			}
			else
			{
				resolved_ = false;
			}
		}

		Collisionable(Uint32 height, Uint32 width, CollisionType type = CollisionType::STATIC)
		{
			component_type = ComponentTypeEnum::COLLISION;
			height_ = height;
			width_ = width;
			
			buffer_pixels = 0;
			width_ = 0;
			height_ = 0;

			collision_type_ = type;
			if (type == CollisionType::STATIC)
			{
				resolved_ = true;
			}
			else
			{
				resolved_ = false;
			}
			
		}
		//SDL_Rect collision_box_;
		CollisionType collision_type_ = CollisionType::UNKNOWN;
		CollisionTag collision_tag_ = CollisionTag::UNKNOWN;
		int buffer_pixels = 0;//"halo" around the collision box making it collide larger/smaller than it really is
		bool resolved_ = false; //set to true whenever this object is checked by collision system. 
		Uint32 radius_ = 0;
		Uint32 width_ = 0;
		Uint32 height_ = 0;

	};

	//todo enum to string for collision_tag
	inline void to_json(json& j, const Collisionable& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::COLLISION]},
			{"collision_type_", p.collision_type_},
			{"collision_tag_", p.collision_tag_},
			{"buffer_pixels", p.buffer_pixels},
			{"resolved_", p.resolved_},
			{"radius_", p.radius_},
			{"width_", p.width_},
			{"height_", p.height_}
		};
	}

	inline void from_json(const json& j, Collisionable& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		j.at("collision_type_").get_to(p.collision_type_);
		j.at("collision_tag_").get_to(p.collision_tag_);
		j.at("buffer_pixels").get_to(p.buffer_pixels);
		j.at("resolved_").get_to(p.resolved_);
		j.at("radius_").get_to(p.radius_);
		j.at("width_").get_to(p.width_);
		j.at("height_").get_to(p.height_);
	}
}