#pragma once

#include <ECS/Components/EventComponent.hpp>
namespace SXNGN::ECS {


	template<typename T>
	struct DropEntry
	{

		int weight; //likelihood of being chosen
		int max_weight;
		int min_weight;
		int reoccurance_penalty;//subtract from weight when chosen
		int accumulation;//add to weight at event tick
		std::vector<DropEntry> children;
		T value;
		bool init = false;
		DropEntry();

		DropEntry<T>* find_event_by_type(T search_value);


		/// <summary>
		/// generate_event
		/// Recursive function that traverses the DropEntry's children (self inclusive) and chooses a DropEntry to return based on its probability weight.
		/// The chosen DropEntry has its probability reduced by its reoccurance_penalty.
		/// Any DropEntry not chosen in the same level in the nodes being considered has its weight increased by accumulation.
		/// The intention is that chosen DropEntrys have less future probability of occurring while not-chosen DropEntrys have their future probability increased.
		/// </summary>
		/// <param name="node"></param>
		/// <returns></returns>
		static DropEntry* generate_event(DropEntry* node);

		static void print_event_table(const std::string& prefix, DropEntry node, bool last_child);

		void print_event_table(DropEntry node);

		std::string to_std_string();

	};

	inline void to_json(json& j, const DropEntry<PartyEventType>& p) {
		j = json{
			{"accumulation", p.accumulation},
			{"max_weight", p.max_weight},
			{"min_weight", p.min_weight},
			{"reoccurance_penalty", p.reoccurance_penalty},
			{"value", party_event_type_enum_to_string()[p.value]},
			{"weight", p.weight},
			{"children", p.children},
			{"init",p.init}
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
		j.at("init").get_to(p.init);
	}
}