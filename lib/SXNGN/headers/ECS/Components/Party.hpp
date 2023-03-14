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
		Party()
		{
			component_type = ComponentTypeEnum::PARTY;
			health_ = 100.0;
			health_max_ = 100.0;
			stamina_ = 1000.0;
			stamina_max_ = stamina_;
			food_max_ = 1000.0;

			hands_ = 1.0;
			muscle_ = 0.0;
			

			lost_counter_ = 0.0;
			sick_counter_ = 0.0;
			weather_counter_ = 0.0;



			inventory_[ItemType::AMMO] = 6;
			inventory_[ItemType::BATTERY] = 1;
			inventory_[ItemType::FOOD] = 750.0;
			inventory_[ItemType::FOOTWEAR] = 1;
			inventory_[ItemType::GPS] = 0;
			inventory_[ItemType::GUN] = 1;
			inventory_[ItemType::MEDKIT] = 1;
			inventory_[ItemType::WATER] = 10;

		}
		
		std::vector<sole::uuid> character_ids_; //uuuid of unique characters who have their own entities
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

		void add_item(ItemType item, double amount)
		{
			if (inventory_.count(item) > 0)
			{
				inventory_[item] += amount;
			}
			else
			{
				inventory_[item] = amount;
			}
		}

		void remove_item(ItemType item, double amount)
		{
			if (inventory_.count(item) > 0)
			{
				inventory_[item] -= amount;
				if (inventory_[item] < 0)
				{
					inventory_.erase(item);
				}
			}
			
		}

		bool can_use_gps(std::string& result)
		{
			if (inventory_[ItemType::BATTERY] > 0 && inventory_[ItemType::GPS] > 0)
			{
				
				result = "Success";
				return true;
			}
			else if (inventory_[ItemType::GPS] < 1)
			{
				result = "No GPS";
			}
			else
			{
				result = "Need batteries";
			}
			return false;
			
		}

		int use_gps(std::string& result)
		{

			if (can_use_gps(result))
			{
				if (lost_counter_ > 0.0)
				{
					lost_counter_ = 0.0;
					remove_item(ItemType::BATTERY, 1);
					result = "Success";
					return 0;
				}
				else
				{
					result = "Not Lost";
					return 1;
				}
			}
			else
			{
				//result already filled in from can_use_gps
				return 1;
			}
		}

		int use_medkit(std::string& result)
		{
			if (can_use_medit(result))
			{
				if (sick_counter_ > 0.0)
				{
					result = "Cured sickness with medkit.";
					sick_counter_ = 0.0;
					remove_item(ItemType::MEDKIT, 1);
					return 0;
				}
				else if (health_ < health_max_)
				{
					health_ = health_max_;
					remove_item(ItemType::MEDKIT, 1);
					result = "Recovered health with medkit.";
					return 0;
				}
			}
			else
			{
				//result filled in from can_use_medkit
				return 1;
			}
			
		}

		bool can_use_medit(std::string& result)
		{
			if (inventory_[ItemType::MEDKIT] > 0)
			{
				result = "Success";
				return true;
			}
			else
			{
				result = "No Medkits";
				return false;
			}
		}

		bool have_footwear(std::string& result)
		{
			return inventory_[ItemType::FOOTWEAR] >= hands_;
		}

		int hands_without_footwear(std::string& result)
		{
			//10 guys and 3 boots = 7 without boots
			//10 guys and 12 boots = -2 without boots
			int temp = hands_ - inventory_[ItemType::FOOTWEAR];
			if (temp > 0)
			{
				return temp;
			}
			else
			{
				return 0;
			}
		}

		int get_fighting_strength(std::string& result)
		{
			//hand with a gun = 10 strength
			//hand with no gun = 2 strength
			//gun needs at least 1 bullet per combat round to fight

			int strength = 0;
			int guns_temp = 0;
			result = "Fighting Strength\n";
			if (inventory_[ItemType::AMMO] > inventory_[ItemType::GUN])
			{
				result += "Ammo for all guns.\n";
				//if more ammo than guns, all guns operational
				guns_temp = inventory_[ItemType::GUN];
			}
			else
			{
				//else if we have say 5 bullets and 10 guns, there are really only 5 guns available
				result += "Ammo for " + std::to_string(inventory_[ItemType::AMMO]) + " guns.\n";
				guns_temp = inventory_[ItemType::AMMO];
			}
			if (guns_temp >= hands_)
			{
				//all hands have guns, 
				result += "Guns for all hands.\n";
				strength += hands_ * 10;
			}
			else if (hands_ > guns_temp)
			{
				result += "Guns for " + std::to_string(guns_temp) + " hands\n";
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