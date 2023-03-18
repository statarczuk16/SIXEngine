#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>
#include <nlohmann/json.hpp>
#include <sole.hpp>
#include <queue>
#include <Constants.h>
#include <ECS/Core/Item.hpp>
using nlohmann::json;

using ComponentTypeEnum = SXNGN::ECS::ComponentTypeEnum;

namespace SXNGN::ECS {

	struct Party : ECS_Component
	{
		Party();
		
		std::vector<sole::uuid> character_ids_; //uuuid of unique characters who have their own entities
		std::vector<sole::uuid> world_location_ids_; //uuids of world locations character can access
		double health_;
		double stamina_;
		double hands_;
		double muscle_;

		double health_max_;
		double stamina_max_;
		double food_max_;

		std::map<ItemType, double> inventory_;
		
		double lost_counter_; //party will make no progress until counter hits 0
		double sick_counter_; //stamina regeneration halved until counter hits 0
		double weather_counter_; //stamina regeneration halved until counter hits 0
		double lost_counter_max_; //party will make no progress until counter hits 0
		double sick_counter_max_; //stamina regeneration halved until counter hits 0
		double weather_counter_max_; //stamina regeneration halved until counter hits 0
		double encumbrance_kg;
		double weight_capacity_kg;
		double overencumbered_mild_thresh_kg_; //if encumbrance is greater than this val, pace slows 
		double overencumbered_medium_thresh_kg_;
		double overencumbered_extreme_thresh_kg_;

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

		int get_fighting_strength(std::string& result);

	};

	inline void to_json(json& j, const Party& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::PARTY]},
			{"character_ids_",p.character_ids_},
			{"health_",p.health_},
			{"stamina_",p.stamina_},
			{"hands_",p.hands_},
			{"muscle_",p.muscle_},
			{"inventory_",p.inventory_}

		};

	}

	inline void from_json(const json& j, Party& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		j.at("character_ids_").get_to(p.character_ids_);
		j.at("health_").get_to(p.health_);
		j.at("stamina_").get_to(p.stamina_);
		j.at("hands_").get_to(p.hands_);
		j.at("muscle_").get_to(p.muscle_);
		j.at("inventory_").get_to(p.inventory_);

	}


}