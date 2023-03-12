#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>
#include <nlohmann/json.hpp>
#include <sole.hpp>
#include <queue>
#include <Constants.h>
using nlohmann::json;

using ComponentTypeEnum = SXNGN::ECS::ComponentTypeEnum;

namespace SXNGN::ECS {

	struct Party : ECS_Component
	{
		Party()
		{
			component_type = ComponentTypeEnum::PARTY;
			health_ = 100.0;
			health_max_ = 100.0;
			stamina_ = 1000.0;
			stamina_max_ = stamina_;
			food_ = 2000.0;
			food_max_ = food_;
			water_ = 100.0;
			hands_ = 1.0;
			muscle_ = 0.0;
			footwear_ = 1;
			lost_counter_ = 0.0;
			sick_counter_ = 0.0;
		}
		
		std::vector<sole::uuid> character_ids_; //uuuid of unique characters who have their own entities
		double health_;
		double stamina_;
		double food_;
		double water_;
		double hands_;
		double muscle_;

		double health_max_;
		double stamina_max_;
		double food_max_;
		int footwear_;
		int batteries_;
		int ammo_;
		int guns_;
		int medkits_;

		bool have_gps_;
		
		double lost_counter_; //party will make no progress until counter hits 0
		double sick_counter_; //stamina regeneration halved until counter hits 0
		double weather_counter_; //stamina regeneration halved until counter hits 0

		bool can_use_gps()
		{
			return batteries_ > 0 && have_gps_;
		}

		bool can_use_medit()
		{
			return medkits_ > 0;
		}

		bool have_footwear()
		{
			return footwear_ >= hands_;
		}

		int hands_without_footwear()
		{
			//10 guys and 3 boots = 7 without boots
			//10 guys and 12 boots = -2 without boots
			int temp = hands_ - footwear_;
			if (temp > 0)
			{
				return temp;
			}
			else
			{
				return 0;
			}
		}

		int get_fighting_strength()
		{
			//hand with a gun = 10 strength
			//hand with no gun = 2 strength
			//gun needs at least 1 bullet per combat round to fight

			int strength;
			int guns_temp;

			if (ammo_ > guns_)
			{
				//if more ammo than guns, all guns operational
				guns_temp = guns_;
			}
			else
			{
				//else if we have say 5 bullets and 10 guns, there are really only 5 guns available
				guns_temp = ammo_;
			}
			if (guns_temp >= hands_)
			{
				//all hands have guns, 
				strength += hands_ * 10;
			}
			else if (hands_ > guns_temp)
			{
				//if not enough guns to go around
				//add 10 for every person with a gun
				int temp = hands_;
				strength += guns_temp * 10;
				temp -= guns_temp;
				//add 2 for the rest without guns
				strength += temp * 2;
			}
			return strength;
		}






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