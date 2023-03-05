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
			weight = 1;
			max_weight = 100;
			min_weight = 0;
			reoccurance_penalty = 100;
			accumulation = 10;
			
		}
		int weight; //likelihood of being chosen
		int max_weight;
		int min_weight;
		int reoccurance_penalty;//subtract from weight when chosen
		int accumulation;//add to weight at event tick
		std::vector<DropEntry> children;
		T value;

		/// <summary>
		/// generate_event
		/// Recursive function that traverses the DropEntry's children (self inclusive) and chooses a DropEntry to return based on its probability weight.
		/// The chosen DropEntry has its probability reduced by its reoccurance_penalty.
		/// Any DropEntry not chosen in the same level in the nodes being considered has its weight increased by accumulation.
		/// The intention is that chosen DropEntrys have less future probability of occurring while not-chosen DropEntrys have their future probability increased.
		/// </summary>
		/// <param name="node"></param>
		/// <returns></returns>
		static DropEntry* generate_event(DropEntry* node)
		{
			//If we are here, this node or one of its children will be chosen, so reduce the future probability of this branch.
			node->weight -= node->reoccurance_penalty;
			node->weight = std::clamp(node->weight, node->min_weight, node->max_weight);
			if (node->children.size() == 0)
			{
				//if this node has no children, this is the end of the function.
				return node;
			}
			else if (node->children.size() == 1)
			{
				//if there is only one child then use it
				return generate_event(&(node->children[0]));
			}
			//algorithm for choosing which event out of this node's children will occur
			//1. sum up total weight of children
			//2. generate random number between 0 and total weight
			//3. Iterate through all children again and accumulate their weight, return the child that makes accumulated weight > random number
			// result is that a child's chance to be chosen is proportional to their weight
			int total_weight = 0;
			for (int i = 0; i < node->children.size(); i++)
			{
				total_weight += node->children[i].weight;
			}
			if (total_weight < 1)
			{
				total_weight = 1;
			}
			int random_number = std::rand() % total_weight;
			int running_weight = 0;
			bool still_searching = true;
			int chosen_event = -1;
			for (int i = 0; i < node->children.size(); i++)
			{
				running_weight += node->children[i].weight;
				if (still_searching && running_weight > random_number)
				{
					chosen_event = i; //save chosen child for later 
					still_searching = false;

				}
				else
				{
					node->children[i].weight += node->children[i].accumulation; //make children that were not chosen more likely to be chosen in the future
					node->children[i].weight = std::clamp(node->children[i].weight, node->children[i].min_weight, node->children[i].max_weight);
				}
			}
			if (still_searching)
			{
				return node;
			}
			else
			{
				return generate_event(&(node->children[chosen_event]));
			}
		}

		static void print_event_table(const std::string& prefix, DropEntry node, bool last_child)
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
						print_event_table(prefix + (last_child ? "    " : "|   "), node.children[i], true);
					}
					else
					{
						print_event_table(prefix + (last_child ? "    " : "|   "), node.children[i], false);
					}
				}

			}
		}

		void print_event_table(DropEntry node)
		{
			print_event_table("", node, false);
		}

		std::string to_std_string()
		{
			return party_event_type_enum_to_string()[value];
		}
	};

	inline void to_json(json& j, const DropEntry<PartyEventType>& p) {
		j = json{
			{"accumulation", p.accumulation},
			{"max_weight", p.max_weight},
			{"min_weight", p.min_weight},
			{"reoccurance_penalty", p.reoccurance_penalty},
			{"value", party_event_type_enum_to_string()[p.value]},
			{"weight", p.weight},
			{"children", p.children}
		};

	}

	inline void from_json(const json& j, DropEntry<PartyEventType>& p) {
		PartyEventType enu = party_event_type_string_to_enum()[j.at("value")];
		p.value = enu;
		j.at("accumulation").get_to(p.accumulation);
		j.at("children").get_to(p.children);
		j.at("max_weight").get_to(p.max_weight);
		j.at("min_weight").get_to(p.min_weight);
		j.at("reoccurance_penalty").get_to(p.reoccurance_penalty);
		j.at("weight").get_to(p.weight);
	}

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
			event_gauge_s_ = 10.0;
			event_tick_s_ = 0.0;
			has_event_table_ = false;

		}
		std::time_t game_clock_;
		double event_gauge_s_; //event generated when event_tick reaches this value
		double event_tick_s_;
		DropEntry<PartyEventType> event_table_;
		bool has_event_table_;


	};

	inline void to_json(json& j, const Director& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::DIRECTOR]},
			{"event_gauge_s_", p.event_gauge_s_},
			{"event_table_", p.event_table_},
			{"event_tick_s_", p.event_tick_s_},
			{"game_clock_", p.game_clock_},
			{"has_event_table_", p.has_event_table_}
		};
	}

	inline void from_json(const json& j, Director& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		j.at("event_gauge_s_").get_to(p.event_gauge_s_);
		j.at("event_table_").get_to(p.event_table_);
		j.at("event_tick_s_").get_to(p.event_tick_s_);
		j.at("game_clock_").get_to(p.game_clock_);
		j.at("has_event_table_").get_to(p.has_event_table_);

	}


}