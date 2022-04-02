#pragma once

#include "ECS/Core/Types.hpp"
#include <array>
#include <cassert>
#include <queue>
#include <SDL.h>

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

			Space(std::string name, std::vector <std::shared_ptr<ExternEntity>> entities)
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

		std::shared_ptr<SDL_Rect> getStateViewPort(ComponentTypeEnum game_state)
		{
			if (game_state <= ComponentTypeEnum::GAME_STATE_START_HERE)
			{
				SDL_LogError(1, "StateManager::getViewPort input is not a game state.");
				return nullptr;
			}
			if (game_state_to_view_port_.count(game_state) != 0)
			{
				return game_state_to_view_port_[game_state];
			}
			else
			{
				SDL_LogDebug(1, "StateManager::getViewPort input has no view port. Creating one based on resolution.");
				std::shared_ptr<SDL_Rect> new_game_state_viewport = std::make_shared<SDL_Rect>();
				
				new_game_state_viewport->x = 0;
				new_game_state_viewport->y = 0;
				new_game_state_viewport->w = this->game_settings.resolution.w;
				new_game_state_viewport->h = this->game_settings.resolution.h;
				game_state_to_view_port_[game_state] = new_game_state_viewport;
				return game_state_to_view_port_[game_state];
			}
		}

		void setStateViewPort(ComponentTypeEnum game_state, std::shared_ptr<SDL_Rect> game_state_rect)
		{
			if (game_state <= ComponentTypeEnum::GAME_STATE_START_HERE)
			{
				SDL_LogError(1, "StateManager::getViewPort input is not a game state.");
				return;
			}
			game_state_to_view_port_[game_state] = game_state_rect;
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
			active_game_states_.sort();
		}

		std::forward_list<ComponentTypeEnum> getActiveGameStates()
		{
			return active_game_states_;
		}

		std::vector< std::shared_ptr<ExternEntity>> retrieveSpaceEntities(std::string state, bool destroy)
		{
			std::vector< std::shared_ptr<ExternEntity>> entity_array;
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

		std::unordered_map < std::string, std::vector<std::vector<sole::uuid>>>& getSpaceToTileMap()
		{
			return space_to_entity_location_map_;
		}

		void addUUIDToLocationMap(int grid_x, int grid_y, sole::uuid uuid, std::string space = SXNGN::DEFAULT_SPACE)
		{
			
			if (space_to_entity_location_map_.count(SXNGN::DEFAULT_SPACE) < 1)
			{
				std::vector<std::vector<sole::uuid> > entity_map(
					getGameSettings()->level_settings.level_width_tiles,
					std::vector<sole::uuid>(getGameSettings()->level_settings.level_height_tiles, SXNGN::BAD_UUID));
				space_to_entity_location_map_[SXNGN::DEFAULT_SPACE] = entity_map;
			}
			space_to_entity_location_map_[SXNGN::DEFAULT_SPACE].at(grid_x).at(grid_y) = uuid;


			if (space_to_entity_location_map_reverse_.count(SXNGN::DEFAULT_SPACE) < 1)
			{
				std::unordered_map<sole::uuid, std::pair<int, int>> new_map;
				space_to_entity_location_map_reverse_[SXNGN::DEFAULT_SPACE] = new_map;
			}
			space_to_entity_location_map_reverse_[SXNGN::DEFAULT_SPACE][uuid] = std::make_pair(grid_x, grid_y);
		}

		int removeUUIDFromLocationMap(sole::uuid uuid, std::string space = SXNGN::DEFAULT_SPACE)
		{

			if (space_to_entity_location_map_reverse_.count(SXNGN::DEFAULT_SPACE) < 1)
			{
				return -1;
			}
			if (space_to_entity_location_map_reverse_.at(DEFAULT_SPACE).count(uuid) < 1)
			{
				return -1;
			}
			auto x_y = space_to_entity_location_map_reverse_[DEFAULT_SPACE].at(uuid);
			space_to_entity_location_map_reverse_[DEFAULT_SPACE].erase(uuid);

			if (space_to_entity_location_map_.count(SXNGN::DEFAULT_SPACE) < 1)
			{
				return -1;
			}

			if (space_to_entity_location_map_.at(SXNGN::DEFAULT_SPACE).at(x_y.first).at(x_y.second) == SXNGN::BAD_UUID)
			{
				return -1;
			}

			space_to_entity_location_map_.at(SXNGN::DEFAULT_SPACE).at(x_y.first).at(x_y.second) = SXNGN::BAD_UUID;

			return 0;

		}

	private:
		std::unordered_map< std::string, Space> spaces;
		std::unordered_map< std::string, std::mutex> spaceGuards;
		std::unordered_map < std::string, std::vector<std::vector<sole::uuid>>> space_to_entity_location_map_;
		std::unordered_map < std::string, std::unordered_map<sole::uuid, std::pair<int,int>>> space_to_entity_location_map_reverse_;
		std::forward_list<ComponentTypeEnum> active_game_states_;
		GameSettings game_settings;
		std::unordered_map<ComponentTypeEnum, std::shared_ptr<SDL_Rect>> game_state_to_view_port_;
		
	};
}