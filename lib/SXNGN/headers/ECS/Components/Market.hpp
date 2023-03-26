#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>



using ComponentTypeEnum = SXNGN::ECS::ComponentTypeEnum;

namespace SXNGN::ECS {


	/// <summary>
	/// Contains data about a ground level tile
	/// </summary>
	struct Market : ECS_Component
	{


		Market()
		{
			component_type = ComponentTypeEnum::MARKET;
		}
		std::map<ItemType, double> inventory_;
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
	};

	inline void to_json(json& j, const Market& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::MARKET]},
			{"inventory_",p.inventory_},
		};
	}

	inline void from_json(const json & j, Market & p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		j.at("inventory_").get_to(p.inventory_);
	};

	


}