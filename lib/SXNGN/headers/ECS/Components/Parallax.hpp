#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>
#include <nlohmann/json.hpp>
#include <sole.hpp>
#include <queue>
#include <Constants.h>
using nlohmann::json;

using ComponentTypeEnum = SXNGN::ECS::A::ComponentTypeEnum;

namespace SXNGN::ECS::A {


	/// <summary>
	/// Contains data about a parallax/scrolling image
	/// Contains a vector of images. The parallax system will lay the location data of these images so that they are end to end.
	/// The parallax system will 
	/// </summary>
	struct Parallax : ECS_Component
	{


		Parallax()
		{
			component_type = ComponentTypeEnum::PARALLAX;
			speed_source_horizontal_ = SXNGN::BAD_STRING_RETURN;
			speed_source_vertical_ = BAD_STRING_RETURN;
			speed_sign_ = 0;
		}
		std::deque<sole::uuid> parallax_images_; //queue of uuids, in order, of the images to display
		std::string speed_source_horizontal_;//database property to get scrolling speed from
		std::string speed_source_vertical_;//database property to get scrolling speed from
		double speed_multiplier_; //multiply value found at speed_source_horizontal_
		bool init_ = false; //parallax system will init the parallax by placing all the images end to end from 0.0
		int speed_sign_; //0 = no preference, match speed source, 1 = always positive, -1 = always negative
		
		

	};

	inline void to_json(json& j, const Parallax& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::PARALLAX]},
			{"parallax_images_", p.parallax_images_},
			{"speed_source_horizontal_", p.speed_source_horizontal_},
			{"speed_source_vertical_", p.speed_source_vertical_},
			{"speed_sign_", p.speed_sign_},
			{"speed_multiplier_", p.speed_multiplier_}
		};

	}

	inline void from_json(const json& j, Parallax& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		auto uuid_arr = j.at("parallax_images_");
		for (auto element : uuid_arr)
		{
			sole::uuid input = sole::rebuild(element);
			std::string test = input.str();
			p.parallax_images_.push_back(input);	
		}

		j.at("speed_source_horizontal_").get_to(p.speed_source_horizontal_);
		j.at("speed_source_vertical_").get_to(p.speed_source_vertical_);
		j.at("speed_sign_").get_to(p.speed_sign_);
		j.at("speed_multiplier_").get_to(p.speed_multiplier_);


	}


}