#pragma once

#include <SDL.h>
#include <ECS/Components/Components.hpp>
#include <ECS/Core/Component.hpp>
#include <nlohmann/json.hpp>
#include <sole.hpp>
#include <queue>
#include <iostream>
#include <Constants.h>
using nlohmann::json;

using ComponentTypeEnum = SXNGN::ECS::ComponentTypeEnum;

namespace SXNGN::ECS {


	template<typename T>
	struct DropEntry
	{
		DropEntry()
		{
			weight = -1;
			
		}
		int weight; //likelihood of being chosen
		int max_weight;
		int min_weight;
		int reoccurance_penalty;//subtract from weight when chosen
		int accumulation;//add to weight at event tick
		std::vector<DropEntry> children;
		T value;

		static DropEntry generate_event(DropEntry node)
		{
			if (node.children.size() == 0)
			{
				return node;
			}
			int total_weight = 0;
			for (int i = 0; i < node.children.size(); i++)
			{
				total_weight += node.children[i].weight;
			}
			int random_number = std::rand() % total_weight;
			int running_weight = 0;
			for (int i = 0; i < node.children.size(); i++)
			{
				running_weight += node.children[i].weight;
				if (running_weight > random_number)
				{
					return generate_event(node.children[i]);
				}
			}
			return node;
		}

		static void to_std_string(const std::string& prefix, DropEntry node, bool last_child)
		{
			
			if (node.weight != -1)
			{
				std::cout << prefix;
				std::cout << (last_child ? "L--" : "|--");
				std::cout << party_event_type_enum_to_string()[node.value] << "-" << node.weight << std::endl;

				for(int i = 0; i < node.children.size(); i++)
				{
					if (i == node.children.size() - 1)
					{
						to_std_string(prefix + (last_child ? "    " : "|   "), node.children[i], true);
					}
					else
					{
						to_std_string(prefix + (last_child ? "    " : "|   "), node.children[i], false);
					}
				}

			}
		}

		void to_std_string(DropEntry node)
		{
			to_std_string("", node, false);
		}
	};

	struct Director : ECS_Component
	{
		Director()
		{
			component_type = ComponentTypeEnum::DIRECTOR;

			struct tm t2;
			t2 = { 0 };
		
			t2.tm_year = 208;
			t2.tm_hour = 12;
			t2.tm_min = 1;
			t2.tm_mon = 1;
			t2.tm_mday = 1;
			t2.tm_sec = 1;
			
			game_clock_ = mktime(&t2);	
		}
		std::time_t game_clock_;
		std::time_t time_since_disaster_;
		DropEntry<PartyEventType> event_table_;


	};

	inline void to_json(json& j, const Director& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::DIRECTOR]},
			

		};
	}

	inline void from_json(const json& j, Director& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
	}


}