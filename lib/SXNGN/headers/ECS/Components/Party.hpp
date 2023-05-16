#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>
#include <nlohmann/json.hpp>
#include <sole.hpp>
#include <queue>
#include <Constants.h>
#include <ECS/Core/Item.hpp>
#include <ECS/Components/EventComponent.hpp>
using nlohmann::json;

using ComponentTypeEnum = SXNGN::ECS::ComponentTypeEnum;

namespace SXNGN::ECS {

	struct Party : ECS_Component
	{
		Party();
		
		std::vector<sole::uuid> character_ids_; //uuuid of unique characters who have their own entities
		std::vector<sole::uuid> world_location_ids_; //uuids of world locations character can access
		double health_ = 0.0;
		double stamina_ = 0.0;
		double hands_ = 0.0;
		double pace_abs_ = 0.0;
		double encumbrance_kg_ = 0.0;
		double encumbrance_real_kg_ = 0.0; //after accounting for sickness etc
		double health_max_ = 0.0;
		double stamina_max_ = 0.0;
		double pace_m_s_ = 0.0; //accounts for encumbrance and sickness
		double pace_real_m_s_ = 0.0; //accounts for encumbrance and sickness and road condition

		std::map<ItemType, double> inventory_;
		
		double lost_counter_ = 0.0; //party will make no progress until counter hits 0
		double sick_counter_s_ = 0.0; //encumbrance multiplied until counter hits 0
		double weather_counter_s_ = 0.0; //take damage until hits 0
		double lost_counter_max_ = 0.0; //party will make no progress until counter hits 0
		double sick_counter_max_ = 0.0; //stamina regeneration halved until counter hits 0
		double weather_counter_max_ = 0.0; //stamina regeneration halved until counter hits 0

		EventSeverity sick_level_ = EventSeverity::UNKNOWN;
		EventSeverity weather_level_ = EventSeverity::UNKNOWN;

		
		double weight_capacity_kg_ = 0.0;
		double overencumbered_mild_thresh_kg_ = 0.0; //if encumbrance is greater than this val, pace slows 
		double overencumbered_medium_thresh_kg_ = 0.0;
		double overencumbered_extreme_thresh_kg_ = 0.0;
		double encumbrance_penalty_m_s_ = 0.0;

		void update_player_ui();

		void update_encumbrance();

		void update_encumbrance_threshs();
	
		void add_item(ItemType item, double amount);

		void remove_item(ItemType item, double amount);
		
		bool can_use_gps(std::string& result);

		int use_gps(std::string& result);

		int use_medkit(std::string& result);

		bool can_use_medit(std::string& result);

		bool have_footwear(std::string& result);

		int hands_without_footwear(std::string& result);

		int get_fighting_strength(std::string& result) const;

		bool can_use_item(ItemType item, std::string& result);

		int use_item(ItemType item, std::string& result);

	};

	inline void to_json(json& j, const Party& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::PARTY]},
			{"character_ids_",p.character_ids_},
			{"world_location_ids_",p.world_location_ids_},
			{"health_",p.health_},
			{"pace_abs_",p.pace_abs_},
			{"stamina_",p.stamina_},
			{"hands_",p.hands_},
			{"encumbrance_kg_",p.encumbrance_kg_},
			{"encumbrance_real_kg_",p.encumbrance_real_kg_},
			{"health_max_",p.health_max_},
			{"stamina_max_",p.stamina_max_},
			{"inventory_",p.inventory_},
			{"lost_counter_",p.lost_counter_},
			{"sick_counter_s_",p.sick_counter_s_},
			{"weather_counter_s_",p.weather_counter_s_},
			{"lost_counter_max_",p.lost_counter_max_},
			{"sick_counter_max_",p.sick_counter_max_},
			{"weather_counter_max_",p.weather_counter_max_},
			{"sick_level_",p.sick_level_},
			{"weather_level_",p.weather_level_},
			{"weight_capacity_kg_",p.weight_capacity_kg_},
			{"overencumbered_mild_thresh_kg_",p.overencumbered_mild_thresh_kg_},
			{"overencumbered_medium_thresh_kg_",p.overencumbered_medium_thresh_kg_},
			{"overencumbered_extreme_thresh_kg_",p.overencumbered_extreme_thresh_kg_},
			{"encumbrance_penalty_m_s_",p.encumbrance_penalty_m_s_},
			{"pace_m_s_",p.pace_m_s_},
			{"pace_real_m_s_",p.pace_real_m_s_}
		};

	}

	inline void from_json(const json& j, Party& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		j.at("character_ids_").get_to(p.character_ids_);
		j.at("world_location_ids_").get_to(p.world_location_ids_);
		j.at("health_").get_to(p.health_);
		j.at("pace_abs_").get_to(p.pace_abs_);
		j.at("stamina_").get_to(p.stamina_);
		j.at("hands_").get_to(p.hands_);
		j.at("encumbrance_kg_").get_to(p.encumbrance_kg_);
		j.at("encumbrance_real_kg_").get_to(p.encumbrance_real_kg_);
		j.at("health_max_").get_to(p.health_max_);
		j.at("stamina_max_").get_to(p.stamina_max_);
		j.at("inventory_").get_to(p.inventory_);
		j.at("lost_counter_").get_to(p.lost_counter_);
		j.at("sick_counter_s_").get_to(p.sick_counter_s_);
		j.at("weather_counter_s_").get_to(p.weather_counter_s_);
		j.at("lost_counter_max_").get_to(p.lost_counter_max_);
		j.at("sick_counter_max_").get_to(p.sick_counter_max_);
		j.at("weather_counter_max_").get_to(p.weather_counter_max_);
		j.at("sick_level_").get_to(p.sick_level_);
		j.at("weather_level_").get_to(p.weather_level_);
		j.at("weight_capacity_kg_").get_to(p.weight_capacity_kg_);
		j.at("overencumbered_mild_thresh_kg_").get_to(p.overencumbered_mild_thresh_kg_);
		j.at("overencumbered_medium_thresh_kg_").get_to(p.overencumbered_medium_thresh_kg_);
		j.at("overencumbered_extreme_thresh_kg_").get_to(p.overencumbered_extreme_thresh_kg_);
		j.at("encumbrance_penalty_m_s_").get_to(p.encumbrance_penalty_m_s_);
		j.at("pace_m_s_").get_to(p.pace_m_s_);
		j.at("pace_real_m_s_").get_to(p.pace_real_m_s_);
		

	}


}