#pragma once

#include <ECS/Core/Coordinator.hpp>
#include <Constants.h>
#include <filesystem>
#include <ECS/Components/Components.hpp>
#include <ECS/Core/Coordinator.hpp>
#include <ECS/Utilities/Entity_Builder_Utils.hpp>
#include <climits>
namespace SXNGN {
	namespace ECS {
		namespace A {

			struct A_Star_Node
			{
				A_Star_Node()
				{
					cost_ = INT_MAX;
					manhattan_cost_ = INT_MAX;
					expanded_ = false;
					grid_x_ = -1;
					grid_y_ = -1;
				}
				A_Star_Node* parent_;
				int cost_;
				int manhattan_cost;
				bool expanded_;
				int grid_x_;
				int grid_y_;
				Entity 
			};

			class Map_Utils
			{
			public:


				static std::tuple<std::vector<Pre_Renderable>, std::vector<Collisionable>, std::vector<Tile>> CreateTileMap(int tile_chunks_width, int tile_chunks_height, std::string tileset, std::string base_tile);
				
				static void CreateNewWorld();

				static void StartNewGame();

				static Uint32 GetDistance(NAVIGATION_TYPE method, Location start, Location end);

				static std::queue<Location> GetPath(NAVIGATION_TYPE method, Location start, Location end);
				
				static std::pair<double, double> GetVector(Location start, Location end);

			};
		}
	}
}