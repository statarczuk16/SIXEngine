#pragma once

#include <SDL.h>
#include <ECS/Components/Components.hpp>
#include <ECS/Core/Component.hpp>
#include <nlohmann/json.hpp>
#include <sole.hpp>
#include <queue>
#include <iostream>
#include <Constants.h>
#include <ECS/Utilities/DropEntry.hpp>
using nlohmann::json;

using ComponentTypeEnum = SXNGN::ECS::ComponentTypeEnum;

namespace SXNGN::ECS {

	struct Director : ECS_Component
	{
		Director();
		std::time_t game_clock_;
		double event_gauge_s_; //event generated when event_tick reaches this value
		double event_tick_s_;
		DropEntry<PartyEventType> event_table_; 
		
		std::unordered_map<PartyEventType, DropEntry<PartyEventType>* > event_table_cache_;

	};

	inline void to_json(json& j, const Director& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::DIRECTOR]},
			{"event_gauge_s_", p.event_gauge_s_},
			{"event_table_", p.event_table_},
			{"event_tick_s_", p.event_tick_s_},
			{"game_clock_", p.game_clock_}
			
		};
	}

	inline void from_json(const json& j, Director& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		j.at("event_gauge_s_").get_to(p.event_gauge_s_);
		j.at("event_table_").get_to(p.event_table_);
		j.at("event_tick_s_").get_to(p.event_tick_s_);
		j.at("game_clock_").get_to(p.game_clock_);
		

	}
}