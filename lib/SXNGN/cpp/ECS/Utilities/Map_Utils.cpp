#include <ECS/Utilities/Map_Utils.hpp>
#include <queue>

namespace SXNGN::ECS::A {
	std::tuple<std::vector<Pre_Renderable*>, std::vector<Collisionable*>, std::vector<Location*>, std::vector<Tile*>> Map_Utils::CreateTileMap(int tile_chunks_width, int tile_chunks_height, std::string tileset, std::string base_tile)
	{
		std::vector<Pre_Renderable*> pre_renders;
		std::vector<Collisionable*> collisionables;
		std::vector<Tile*> tiles;
		std::vector<Location*> locations;
		int tiles_high = tile_chunks_height * TILES_TO_CHUNK_EDGE;
		int tiles_wide = tile_chunks_width * TILES_TO_CHUNK_EDGE;
		for (int h = 0; h < tiles_high; h++)
		{
			for (int w = 0; w < tiles_wide; w++)
			{
				Sint32 x_pixels = w * SXNGN::BASE_TILE_WIDTH;
				Sint32 y_pixels = h * SXNGN::BASE_TILE_HEIGHT;
				SDL_Rect collision_box;
				collision_box.x = x_pixels;
				collision_box.y = y_pixels;
				collision_box.w = SXNGN::BASE_TILE_WIDTH;
				collision_box.h = SXNGN::BASE_TILE_HEIGHT;
				Pre_Renderable* pre_render = nullptr;
				Collisionable* collision = nullptr;
				Tile* tile;
				Location* location = nullptr;
				if (h == 0 || h == tiles_high - 1 || w == 0 || w == tiles_wide - 1)
				{
					pre_render = new Pre_Renderable(tileset, "BLACK_BORDER", A::RenderLayer::GROUND_LAYER);
					collision = new Collisionable(SXNGN::BASE_TILE_WIDTH, SXNGN::BASE_TILE_HEIGHT, CollisionType::STATIC, -1);
					location = new Location(x_pixels, y_pixels);
					tile = new Tile(-1);
					
					//tile = Entity_Builder_Utils::Create_Tile(w, h);
				}
				else
				{
					pre_render = new Pre_Renderable(tileset, base_tile, A::RenderLayer::GROUND_LAYER);
					tile = new Tile(1);
					location = new Location(x_pixels, y_pixels);
					//collision = Entity_Builder_Utils::Create_Collisionable(collision_box,  CollisionType::NONE);
					//tile = Entity_Builder_Utils::Create_Tile(w, h);
				}
				
				pre_renders.push_back(pre_render);
				locations.push_back(location);
				collisionables.push_back(collision);
				tiles.push_back(tile);
			}
		}
		return std::make_tuple(pre_renders, collisionables, locations, tiles);
	}

	void Map_Utils::CreateNewWorld()
	{
		std::shared_ptr<Coordinator> gCoordinator = Database::get_coordinator();
	
		ComponentTypeEnum state = ComponentTypeEnum::OVERWORLD_STATE;
		auto settings = gCoordinator->get_state_manager()->getGameSettings();
		std::tuple<std::vector<Pre_Renderable*>, std::vector<Collisionable*>, std::vector<Location*>, std::vector<Tile*>> game_map = CreateTileMap(settings->level_settings.level_width_chunks, settings->level_settings.level_height_chunks, "APOCALYPSE_MAP", "ROCK_GROUND");

		auto game_map_pre_renders = std::get<0>(game_map);
		auto game_map_collisionables = std::get<1>(game_map);
		auto game_map_locations = std::get<2>(game_map);
		auto game_map_tiles = std::get<3>(game_map);

		for (int i = 0; i < game_map_pre_renders.size(); i++)
		{
			auto map_tile_entity = gCoordinator->CreateEntity();
			Pre_Renderable* pre_render = (game_map_pre_renders.at(i)); 
			if (pre_render)
			{
				gCoordinator->AddComponent(map_tile_entity, pre_render);
			}
			
			
			Collisionable* collisionable =(game_map_collisionables.at(i));
			if (collisionable)
			{
				gCoordinator->AddComponent(map_tile_entity, collisionable);
			}
			
			Location* location = (game_map_locations.at(i));
			if (location)
			{
				gCoordinator->AddComponent(map_tile_entity, location);
			}

			Tile* tile = (game_map_tiles.at(i));
			if (tile)
			{
				gCoordinator->AddComponent(map_tile_entity, tile);
			}
			
			gCoordinator->AddComponent(map_tile_entity, Create_Gamestate_Component_from_Enum(state));
		}

		Entity test_person = Entity_Builder_Utils::Create_Person(*gCoordinator, state, 4, 8, "APOCALYPSE_MAP", "GUNMAN_1", true, "Player");

		Entity test_worker = Entity_Builder_Utils::Create_Person(*gCoordinator, state, 4, 4, "APOCALYPSE_MAP", "GUNMAN_2", false, "Worker");

		auto camera = CameraComponent::get_instance();
		camera->set_target(test_person);

	}

	void Map_Utils::StartNewGame()
	{
		std::shared_ptr<Coordinator> gCoordinator = Database::get_coordinator();

		std::forward_list<ComponentTypeEnum> active_game_states;
		active_game_states.push_front(ComponentTypeEnum::MAIN_GAME_STATE);
		active_game_states.push_front(ComponentTypeEnum::CORE_BG_GAME_STATE);
		gCoordinator->GameStateChanged(active_game_states);

		CreateNewWorld();

	}

	Uint32 Map_Utils::GetDistance(NAVIGATION_TYPE method, Coordinate start, Coordinate end)
	{
		switch (method)
		{
		case NAVIGATION_TYPE::MANHATTAN:
		{
			return abs(start.x - end.x) + abs(start.y - end.y);
			break;
		}
		default:
		{
			SDL_LogError(1, "GetDistance: Only Manhattan supported now.");
			return -1;
			break;
		}
		}
	}

	std::queue<Coordinate> Map_Utils::GetPath(NAVIGATION_TYPE method, Coordinate start, Coordinate end)
	{
		
		std::queue<Coordinate> q;
		switch (method)
		{
		case NAVIGATION_TYPE::MANHATTAN:
		{
			q.push(end);
			return q;
			break;
		}
		case NAVIGATION_TYPE::A_STAR:
		{
			//min priority queue


			std::priority_queue<A_Star_Node*, std::vector<A_Star_Node*>, AStarNodeGreaterThan> open_nodes;
			std::vector<A_Star_Node*> closed_nodes;
			auto gCoordinator = *SXNGN::Database::get_coordinator();
			auto collision_map_all = gCoordinator.getCollisionMap();
			auto collision_map = collision_map_all.at(SXNGN::DEFAULT_SPACE);
			std::vector<std::vector<A_Star_Node*> > a_star_map(
				collision_map.size(),
				std::vector<A_Star_Node*>(collision_map[0].size(), nullptr));


			size_t start_grid_x = start.x / SXNGN::BASE_TILE_WIDTH;
			size_t start_grid_y = start.y / SXNGN::BASE_TILE_HEIGHT;
			size_t end_grid_x = end.x / SXNGN::BASE_TILE_WIDTH;
			size_t end_grid_y = end.y / SXNGN::BASE_TILE_HEIGHT;

			if (start_grid_x > collision_map.size() || start_grid_y > collision_map[start_grid_x].size())
			{
				SDL_LogDebug(1, "A* Path Navigation Failed out of bounds start point");
				
				return q;
			}
			if (end_grid_x > collision_map.size() || end_grid_y > collision_map[end_grid_x].size())
			{
				SDL_LogDebug(1, "A* Path Navigation Failed out of bounds end point ");
				
				return q;
			}
			if (collision_map[end_grid_x][end_grid_y] == -1 || collision_map[start_grid_x][start_grid_y] == -1)
			{
				SDL_LogDebug(1, "A* Path Navigation Failed Unreachable Destination ");
				
				return q;
			}

			

			A_Star_Node* start_node = new A_Star_Node(start_grid_x, start_grid_y, end_grid_x, end_grid_y, collision_map[start_grid_x][start_grid_y]);
			A_Star_Node* final_node = nullptr;
			


			start_node->fx_estimated_total_cost_ = 0;
			start_node->gx_cost_to_this_node_ = 0;
			start_node->hx_manhattan_cost_ = 0;


			a_star_map[start_node->grid_x_][start_node->grid_y_] = start_node;
			open_nodes.push(start_node);
			int return_code = 2;
			std::vector<int> visit_x_vals = { -1, 0, 1 };
			std::vector<int> visit_y_vals = { -1, 0, 1 };
			std::cout << "AStar From " << start_grid_x << " , " << start_grid_y << " to " << end_grid_x << " , " << end_grid_y << std::endl;
			A_Star_Node* visiting_node = nullptr;
			while (!open_nodes.empty())
			{
				visiting_node = open_nodes.top();
				open_nodes.pop();
				if (visiting_node->grid_x_ == end_grid_x && visiting_node->grid_y_ == end_grid_y)
				{
					final_node = visiting_node;
					return_code = 0;
					break;
				}
				
				/**
				std::cout << " --- Visiting Node " << visiting_node->grid_x_ << " , " << visiting_node->grid_y_
					<< " FX: " << visiting_node->fx_estimated_total_cost_
					<< " GX: " << visiting_node->gx_cost_to_this_node_
					<< " HX: " << visiting_node->hx_manhattan_cost_
					<< std::endl;
				**/
				//visit each neighbor of node
				for (const int visit_x : visit_x_vals)
				{
					for (const int visit_y : visit_y_vals)
					{
						//dont visit yourself (0,0) or any diagonal moves (1,1) (-1,1) (1,-1) (-1,-1) because cannot move diagonally through a solid block
						if (abs(visit_x) == abs(visit_y))
						{
							continue;
						}

						if (visit_x == 0 && visit_y == 0)
						{
							continue; //don't visit self
						}



						int node_to_visit_x = visiting_node->grid_x_ + visit_x;
						int node_to_visit_y = visiting_node->grid_y_ + visit_y;
						//check neighbor is in bounds of grid
						if (node_to_visit_x >= 0 && node_to_visit_x < a_star_map.size()
							&& node_to_visit_y >= 0 && node_to_visit_y < a_star_map[node_to_visit_x].size())
						{

							A_Star_Node* node_to_visit = a_star_map[node_to_visit_x][node_to_visit_y];
							//if node we want to visit hasn't been initialized, grab its data and make an a* compatible node
							if (node_to_visit == nullptr)
							{
								node_to_visit = new A_Star_Node(
									node_to_visit_x,
									node_to_visit_y,
									end_grid_x,
									end_grid_y,
									collision_map[node_to_visit_x][node_to_visit_y]);
								a_star_map[node_to_visit_x][node_to_visit_y] = node_to_visit;
							}

							//if the node to visit has already been expanded (has been the visiting node), no better bath will be found through it.
							if (node_to_visit->expanded_ == true)
							{
								continue;
							}
							bool found_better_path = false;

							if (node_to_visit->visited_ == true)
							{
								found_better_path = node_to_visit->check_if_better_path(visiting_node);
							}
							else
							{
								found_better_path = node_to_visit->check_if_better_path(visiting_node);
								open_nodes.push(node_to_visit);
							}
							
							/**
							std::cout << " ------ Visiting " << node_to_visit_x << " , " << node_to_visit_y
								<< " FX: " << node_to_visit->fx_estimated_total_cost_
								<< " GX: " << node_to_visit->gx_cost_to_this_node_
								<< " HX: " << node_to_visit->hx_manhattan_cost_
								<< " better path? " << found_better_path << std::endl;
							**/
							if (found_better_path)
							{
								node_to_visit->parent_ = (a_star_map[visiting_node->grid_x_][visiting_node->grid_y_]);
							}

						}
					}
				} //end for loop for visiting all neighbor nodes of visiting node
				visiting_node->expanded_ = true;
				a_star_map[visiting_node->grid_x_][visiting_node->grid_y_] = visiting_node;

				closed_nodes.push_back(visiting_node);
			} //end loop no more open nodes

			if (final_node == nullptr || final_node->grid_x_ != end_grid_x || final_node->grid_y_ != end_grid_y)
			{
				SDL_LogError(1, "A* Path Navigation Failed to get to End Node");
				

			}
			else
			{
				// Unwind the final node to get the full path A* will use
				std::vector<Coordinate> temp_queue;
				A_Star_Node* parent = final_node;
				int iter = 0;
				size_t size_map = a_star_map[0].size() * a_star_map.size();
				while (parent != nullptr && iter < size_map)
				{
					parent->is_solution_node_ = true;
					Coordinate solution_node = Coordinate(parent->grid_x_ * SXNGN::BASE_TILE_WIDTH, parent->grid_y_ * SXNGN::BASE_TILE_HEIGHT);
					temp_queue.push_back(solution_node);
					parent = parent->parent_;
					iter += 1;
				}
				if (iter > size_map - 1)
				{
					SDL_LogError(1, "A* Path Navigation Failed resolve path within iteration limit");
					q = std::queue<Coordinate>();
					

				}
				for (int i = temp_queue.size() - 1; i >= 0; i--)
				{
					SDL_LogDebug(1, "Waypoint Generated: %d, %d", temp_queue[i].x / SXNGN::BASE_TILE_WIDTH, temp_queue[i].y / SXNGN::BASE_TILE_HEIGHT);
					q.push(temp_queue[i]);
				}

				SDL_LogDebug(1, "Printing Collision Map");
				for (int i = 0; i < collision_map.size(); i++)
				{
					for (int j = 0; j < collision_map[i].size(); j++)
					{
						if (a_star_map[j][i] == nullptr)
						{
							//std::cout << std::setw(3) << collision_map[j][i];
							std::cout << std::setw(3) << ".";
						}
						else if (a_star_map[j][i]->is_solution_node_)
						{
							std::cout << std::setw(3) << 'X';
						}
						else
						{
							std::cout << std::setw(3) << a_star_map[j][i]->traversal_cost;
						}
						
					}
					std::cout << std::endl;
				}
			}

			for (A_Star_Node* node : closed_nodes)
			{
				delete node;
			}
			if (final_node != nullptr)
			{
				delete final_node;
			}

			

			
			return q;


			

		}
		default:
		{
			SDL_LogError(1, "GetDistance: Only Manhattan supported now.");
			return q;
			break;
		}
		}
	}

	std::pair<double, double>  Map_Utils::GetVector(SXNGN::ECS::A::Coordinate start, SXNGN::ECS::A::Coordinate end)
	{
		double distance = ECS::A::Map_Utils::GetDistance(ECS::A::NAVIGATION_TYPE::MANHATTAN, start, end);
		if (distance == 0)
		{
			return std::make_pair(0, 0);
		}
		int x = end.x - start.x;
		int y = end.y - start.y;
		std::pair<double, double> result = std::make_pair(x / distance, y / distance);
		return result;
	}

}