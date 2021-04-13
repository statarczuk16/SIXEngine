#pragma once

#include "ECS/Core/Types.hpp"
#include <array>
#include <cassert>
#include <queue>

namespace SXNGN::ECS::A {

	//Chunks are 16x16 tiles
	
	struct LevelGenerationSettings {
		Uint32 level_width_tiles;
		Uint32 level_height_tiles;
		Uint32 level_width_chunks;
		Uint32 level_height_chunks;
	};

	struct GameSettings {
		SDL_Rect resolution;
		LevelGenerationSettings level_settings;
	};

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

		void setLevelWidthTiles(Uint32 width)
		{
			this->game_settings.level_settings.level_width_chunks = width;
			this->game_settings.level_settings.level_width_tiles = width * TILES_TO_CHUNK_EDGE;
		}

		void setLevelHeightTiles(Uint32 height)
		{
			this->game_settings.level_settings.level_height_chunks = height;
			this->game_settings.level_settings.level_height_tiles = height * TILES_TO_CHUNK_EDGE;
		}

		void setResolution(SDL_Rect resolution)
		{
			this->game_settings.resolution = resolution;
		}

		const GameSettings* getGameSettings()
		{
			return &game_settings;
		}

		

		StateManager()
		{
			this->setLevelHeightTiles(DEFAULT_WORLD_HEIGHT_CHUNKS);
			this->setLevelWidthTiles(DEFAULT_WORLD_WIDTH_CHUNKS);
			SDL_Rect resolution;
			resolution.w = DEFAULT_SCREEN_RES_WIDTH;
			resolution.h = DEFAULT_SCREEN_RES_HEIGHT;
			resolution.x = 0;
			resolution.y = 0;
			this->setResolution(resolution);
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
		GameSettings game_settings;
		
	};
}