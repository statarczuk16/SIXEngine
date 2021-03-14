#pragma once

#include "ECS/Core/Types.hpp"
#include <array>
#include <cassert>
#include <queue>

namespace SXNGN::ECS {

	class StateManager
	{
	public:
		struct Gamestate {
			std::shared_ptr<std::mutex> lock_;
			std::vector <std::shared_ptr<A::ExternEntity>> entities_;
			std::string name_;

			Gamestate(std::string name, std::vector <std::shared_ptr<A::ExternEntity>> entities)
			{
				entities_ = entities;
				name_ = name;
				lock_ = std::make_shared<std::mutex>();
			}

			Gamestate()
			{
				printf("Gamestate::Gamestate() Calling this for some reason...\n");//compiler complains about no default constructor without this???
			}
		};

		StateManager()
		{

		}

		void cacheEntityInState(std::shared_ptr<A::ExternEntity> entity_to_store, std::string state = "Temp")
		{

			if (gameStates.count(state) == 0)
			{
				printf("stateManager: Creating new state: %s", state.c_str());
				std::vector< std::shared_ptr<A::ExternEntity>> new_entity_array = { entity_to_store };
				Gamestate new_state(state, new_entity_array);
				gameStates[state] = new_state;
			
			}
			else
			{
				std::lock_guard<std::mutex> guard(*gameStates[state].lock_);
				gameStates[state].entities_.push_back((entity_to_store));
			}
		}

		std::vector< std::shared_ptr<A::ExternEntity>> retrieveStateEntities(std::string state, bool destroy)
		{
			std::vector< std::shared_ptr<A::ExternEntity>> entity_array;
			if (gameStates.count(state) != 0)
			{
				std::shared_ptr<std::mutex> lock = gameStates[state].lock_;
				std::lock_guard<std::mutex> guard(*lock);
				 entity_array = gameStates[state].entities_;
				if (destroy)
				{
					gameStates.erase(state);
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
		std::unordered_map< std::string, Gamestate> gameStates;
		std::unordered_map< std::string, std::mutex> stateGuards;
	};
}