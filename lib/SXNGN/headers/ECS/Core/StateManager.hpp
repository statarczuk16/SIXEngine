#pragma once

#include "ECS/Core/Types.hpp"
#include <array>
#include <cassert>
#include <queue>

namespace SXNGN::ECS::A {

	class StateManager
	{
	public:
		/// <summary>
		/// A space is a location where ECS_Components can live when not inside of the ECS
		/// </summary>
		struct Space {
			std::shared_ptr<std::mutex> lock_;
			std::vector <std::shared_ptr<ExternEntity>> entities_;
			std::string name_;

			Space(std::string name, std::vector <std::shared_ptr<A::ExternEntity>> entities)
			{
				entities_ = entities;
				name_ = name;
				lock_ = std::make_shared<std::mutex>();
			}

			Space()
			{
				
			}
		};

		StateManager()
		{

		}

		void cacheEntityInSpace(std::shared_ptr<ExternEntity> entity_to_store, std::string state = "Temp")
		{

			if (spaces.count(state) == 0)
			{
				printf("stateManager: Creating new space: %s", state.c_str());
				std::vector< std::shared_ptr<A::ExternEntity>> new_entity_array = { entity_to_store };
				Space new_state(state, new_entity_array);
				spaces[state] = new_state;
			
			}
			else
			{
				std::lock_guard<std::mutex> guard(*spaces[state].lock_);
				spaces[state].entities_.push_back((entity_to_store));
			}
		}

		void gameStateChanged(std::forward_list<ComponentTypeEnum> active_game_states)
		{
			active_game_states_.clear();
			active_game_states_ = active_game_states;
		}

		std::forward_list<ComponentTypeEnum> getActiveGameStates()
		{
			return active_game_states_;
		}

		std::vector< std::shared_ptr<ExternEntity>> retrieveSpaceEntities(std::string state, bool destroy)
		{
			std::vector< std::shared_ptr<A::ExternEntity>> entity_array;
			if (spaces.count(state) != 0)
			{
				std::shared_ptr<std::mutex> lock = spaces[state].lock_;
				std::lock_guard<std::mutex> guard(*lock);
				 entity_array = spaces[state].entities_;
				if (destroy)
				{
					spaces.erase(state);
				}
				return entity_array;
			}
			else
			{
				printf("stateManager: No such state: %s", state.c_str());
				return entity_array;
			}
		}


	private:
		std::unordered_map< std::string, Space> spaces;
		std::unordered_map< std::string, std::mutex> spaceGuards;
		std::forward_list<ComponentTypeEnum> active_game_states_;
		
	};
}