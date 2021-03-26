#pragma once

#include <ECS/Core/Coordinator.hpp>
#include <Constants.h>
#include <filesystem>
#include <ECS/Components/Components.hpp>
#include <ECS/Core/Coordinator.hpp>
#include <ECS/Utilities/Entity_Builder_Utils.hpp>
namespace SXNGN {
	namespace ECS {
		namespace A {
			class Map_Utils
			{
			public:
				static std::tuple<std::vector<Pre_Renderable>, std::vector<Collisionable>, std::vector<Tile>> CreateTileMap(int tile_chunks_width, int tile_chunks_height, std::string tileset, std::string base_tile);
				
				static void CreateNewWorld();

				static void StartNewGame();
				

				

			};
		}
	}
}