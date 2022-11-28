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
			flip_1_delete_0 = true;
			snap_source_y_ = SXNGN::BAD_UUID;
			snap_source_y_offset_ = 0;
		}
		std::deque<sole::uuid> parallax_images_; //queue of uuids, in order, of the images to display
		std::string speed_source_horizontal_;//database property to get scrolling speed from
		std::string speed_source_vertical_;//database property to get scrolling speed from
		double speed_multiplier_; //multiply value found at speed_source_horizontal_
		bool init_ = false; //parallax system will init the parallax by placing all the images end to end from 0.0
		int speed_sign_; //0 = no preference, match speed source, 1 = always positive, -1 = always negative
		bool flip_1_delete_0; //if 1, parallax image will roll over when escaping screen bounds, otherwise entity will be removed
		sole::uuid snap_source_y_; //if set, parallax will snap images in the parallax to snap_source_y_'s location + snap_source_y_offset_
		double snap_source_y_offset_;

	};

	inline void to_json(json& j, const Parallax& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::PARALLAX]},
			{"parallax_images_", p.parallax_images_},
			{"speed_source_horizontal_", p.speed_source_horizontal_},
			{"speed_source_vertical_", p.speed_source_vertical_},
			{"speed_sign_", p.speed_sign_},
			{"flip_1_delete_0", p.flip_1_delete_0},
			{"snap_source_y_", p.snap_source_y_},
			{"snap_source_y_offset_", p.snap_source_y_offset_},
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
		j.at("flip_1_delete_0").get_to(p.flip_1_delete_0);
		j.at("snap_source_y_").get_to(p.snap_source_y_);
		j.at("snap_source_y_offset_").get_to(p.snap_source_y_offset_);
		j.at("speed_multiplier_").get_to(p.speed_multiplier_);


	}


}