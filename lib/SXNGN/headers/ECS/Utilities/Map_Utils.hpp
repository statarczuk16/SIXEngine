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
				A_Star_Node(int grid_x, int grid_y, int dest_grid_x, int dest_grid_y, int cost)
				{
					gx_cost_to_this_node_ = INT_MAX;
					fx_estimated_total_cost_ = INT_MAX;
					grid_x_ = grid_x;// x / SXNGN::BASE_TILE_WIDTH;
					grid_y_ = grid_y;// y / SXNGN::BASE_TILE_HEIGHT;
					hx_manhattan_cost_ = abs(grid_x_ - dest_grid_x) + abs(grid_y_ - dest_grid_y);
					expanded_ = false;
					
					
					traversal_cost = cost;
					visited_ = false;
					parent_ = nullptr;
					is_solution_node_ = false;
				}
				A_Star_Node* parent_;
				int gx_cost_to_this_node_; //total cost of from start to this node
				int hx_manhattan_cost_; //estimated cost from this node to destination
				int fx_estimated_total_cost_; //estimated cost from start, through this node, to end (real_cost_ + manhattan_cost_)
				bool expanded_;
				int grid_x_;
				int grid_y_;
				bool visited_;
				int traversal_cost;
				bool is_solution_node_;

				const bool operator < (A_Star_Node* r) const {
					return (fx_estimated_total_cost_ < r->fx_estimated_total_cost_);
				}

				const bool operator > (A_Star_Node* r) const {
					return (fx_estimated_total_cost_ > r->fx_estimated_total_cost_);
				}

				/**
				void init_grid(int grid_x, int grid_y, int dest_grid_x, int dest_grid_y, int cost)
				{
					grid_x_ = grid_x;
					grid_y_ = grid_y;
					traversal_cost = cost;
					
					initialized = true;
				}

				void init_pixel(int pixel_x, int pixel_y, int dest_grid_x, int dest_grid_y, int cost)
				{
					init_grid(pixel_x / SXNGN::BASE_TILE_WIDTH, pixel_y / SXNGN::BASE_TILE_HEIGHT,
						dest_grid_x, dest_grid_y, cost);
				}
				**/

				bool check_if_better_path(const A_Star_Node* visiting_node)
				{
					visited_ = true;
					//if node is not traversible
					if (traversal_cost == 0)
					{
						return false;
					}
					int old_gx = gx_cost_to_this_node_;
					gx_cost_to_this_node_ = visiting_node->gx_cost_to_this_node_ + traversal_cost;
					fx_estimated_total_cost_ = gx_cost_to_this_node_ + hx_manhattan_cost_;
					if (old_gx > gx_cost_to_this_node_)
					{
						return true;
					}
					gx_cost_to_this_node_ = old_gx;
					return false;
				}


			};

			class AStarNodeGreaterThan
			{
			public:
				int operator() (const A_Star_Node* p1, const A_Star_Node* p2)
				{
					return p1->fx_estimated_total_cost_ > p2->fx_estimated_total_cost_;
				}
			};

			class Map_Utils
			{
			public:


				static std::tuple<std::vector<Pre_Renderable*>, std::vector<Collisionable*>, std::vector<Location*>> CreateTileMap(int tile_chunks_width, int tile_chunks_height, std::string tileset, std::string base_tile);
				
				static void CreateNewWorld();

				static void StartNewGame();

				static Uint32 GetDistance(NAVIGATION_TYPE method, Coordinate start, Coordinate end);

				static std::queue<Coordinate> GetPath(NAVIGATION_TYPE method, Coordinate start, Coordinate end);
				
				static std::pair<double, double> GetVector(Coordinate start, Coordinate end);

			};
		}
	}
}