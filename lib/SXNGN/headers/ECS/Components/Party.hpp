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

	struct Party : ECS_Component
	{
		Party()
		{

		}
		
		std::vector<sole::uuid> character_ids_; //uuuid of unique characters who have their own entities
		double health_;
		double stamina_;
		double food_;
		double water_;
		double hands_;
		double muscle_;


	};

	inline void to_json(json& j, const Party& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::PARTY]},
			{"character_ids_",p.character_ids_},
			{"health_",p.health_},
			{"stamina_",p.stamina_},
			{"food_",p.food_},
			{"water_",p.water_},
			{"hands_",p.hands_},
			{"muscle_",p.muscle_}

		};

	}

	inline void from_json(const json& j, Party& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		j.at("character_ids_").get_to(p.character_ids_);
		j.at("health_").get_to(p.health_);
		j.at("stamina_").get_to(p.stamina_);
		j.at("food_").get_to(p.food_);
		j.at("water_").get_to(p.water_);
		j.at("hands_").get_to(p.hands_);
		j.at("muscle_").get_to(p.muscle_);

	}


}