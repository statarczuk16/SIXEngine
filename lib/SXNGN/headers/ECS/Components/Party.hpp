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
			{"character_ids_",character_ids_},
			{"health_",health_},
			{"stamina_",stamina_},
			{"food_",food_},
			{"water_",water_},
			{"hands_",hands_},
			{"muscle_",muscle_}

		};

	}

	inline void from_json(const json& j, Party& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		p.character_ids_ = character_ids_;
		p.health_ = health_;
		p.stamina_ = stamina_;
		p.food_ = food_;
		p.water_ = water_;
		p.hands_ = hands_;
		p.muscle_ = muscle_;

	}


}