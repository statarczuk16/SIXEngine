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
					gx_cost_to_this_node_ = INT_MAX;
					hx_manhattan_cost_ = INT_MAX;
					fx_estimated_total_cost_ = INT_MAX;
					expanded_ = false;
					grid_x_ = -1;// x / SXNGN::BASE_TILE_WIDTH;
					grid_y_ = -1;// y / SXNGN::BASE_TILE_HEIGHT;
					initialized = false;
					traversal_cost = 0;
				}
				A_Star_Node* parent_;
				int gx_cost_to_this_node_; //total cost of from start to this node
				int hx_manhattan_cost_; //estimated cost from this node to destination
				int fx_estimated_total_cost_; //estimated cost from start, through this node, to end (real_cost_ + manhattan_cost_)
				bool expanded_;
				int grid_x_;
				int grid_y_;
				Entity mapped_entity;
				bool initialized;
				int traversal_cost;

				const bool operator < (const A_Star_Node& r) const {
					return (fx_estimated_total_cost_ < r.fx_estimated_total_cost_);
				}

				void init_grid(int grid_x, int grid_y, int dest_grid_x, int dest_grid_y, int cost)
				{
					grid_x_ = grid_x;
					grid_y_ = grid_y;
					traversal_cost = cost;
					hx_manhattan_cost_ = abs(grid_x_ - dest_grid_x) + abs(grid_y_ - dest_grid_y);
				}

				void init_pixel(int pixel_x, int pixel_y, int dest_grid_x, int dest_grid_y, int cost)
				{
					init_grid(pixel_x / SXNGN::BASE_TILE_WIDTH, pixel_y / SXNGN::BASE_TILE_HEIGHT,
						dest_grid_x, dest_grid_y, cost);
				}

				bool visit(const A_Star_Node& visiting_node)
				{
					int old_gx = this->gx_cost_to_this_node_;
					gx_cost_to_this_node_ = visiting_node.gx_cost_to_this_node_ + traversal_cost;
					
				}


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