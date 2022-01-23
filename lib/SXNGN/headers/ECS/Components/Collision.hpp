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
		IMMOVEABLE,
		ELASTIC,
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
			buffer_pixels = 0;
			resolved_ = false;
		}
		SDL_Rect collision_box_;
		CollisionType collision_type_ = CollisionType::UNKNOWN;
		CollisionTag collision_tag_ = CollisionTag::UNKNOWN;
		bool selectable_ = false;
		int buffer_pixels = 0;//"halo" around the collision box making it collide larger/smaller than it really is
		bool resolved_ = false; //set to true whenever this object is checked by collision system. 
		//std::vector<Entity> touching_; //list of entities we know object is touching - reset whenever resolved is changed (object moves)
	};

	//todo enum to string for collision_tag
	inline void to_json(json& j, const Collisionable& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::COLLISION]},
			{"collision_box_.x", p.collision_box_.x},
			{"collision_box_.y", p.collision_box_.y},
			{"collision_box_.w", p.collision_box_.w},
			{"collision_box_.h", p.collision_box_.h},
			{"collision_type_", p.collision_type_},
			{"collision_tag_", p.collision_tag_},
			{"selectable_", p.selectable_},
			{"buffer_pixels", p.buffer_pixels}
		};
	}

	inline void from_json(const json& j, Collisionable& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		j.at("collision_box_.x").get_to(p.collision_box_.x);
		j.at("collision_box_.y").get_to(p.collision_box_.y);
		j.at("collision_box_.w").get_to(p.collision_box_.w);
		j.at("collision_box_.h").get_to(p.collision_box_.h);
		j.at("collision_type_").get_to(p.collision_type_);
		j.at("collision_tag_").get_to(p.collision_tag_);
		j.at("selectable_").get_to(p.selectable_);
		j.at("buffer_pixels").get_to(p.buffer_pixels);
	}
}