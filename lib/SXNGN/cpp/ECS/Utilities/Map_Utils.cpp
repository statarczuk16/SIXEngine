#include <ECS/Utilities/Map_Utils.hpp>

namespace SXNGN::ECS::A {
	std::tuple<std::vector<Pre_Renderable>, std::vector<Collisionable>, std::vector<Tile>> Map_Utils::CreateTileMap(int tile_chunks_width, int tile_chunks_height, std::string tileset, std::string base_tile)
	{
		std::vector<Pre_Renderable> pre_renders;
		std::vector<Collisionable> collisionables;
		std::vector<Tile> tiles;
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
				auto pre_render = new A::Pre_Renderable(x_pixels, y_pixels, tileset, base_tile, A::RenderLayer::GROUND_LAYER);
				auto collision = Entity_Builder_Utils::Create_Collisionable(collision_box, A::CollisionType::NONE);
				auto tile = Entity_Builder_Utils::Create_Tile(w, h);
				pre_renders.push_back(*pre_render);
				collisionables.push_back(*collision);
				tiles.push_back(*tile);
			}
		}
		return std::make_tuple(pre_renders, collisionables, tiles);
	}

	void Map_Utils::CreateNewWorld()
	{
		std::shared_ptr<Coordinator> gCoordinator = Database::get_coordinator();
	
		auto settings = gCoordinator->get_state_manager()->getGameSettings();
		std::tuple<std::vector<Pre_Renderable>, std::vector<Collisionable>, std::vector<Tile>> game_map = CreateTileMap(settings->level_settings.level_width_chunks, settings->level_settings.level_height_chunks, "APOCALYPSE_MAP", "ROCK_GROUND");

		auto game_map_pre_renders = std::get<0>(game_map);
		auto game_map_collisionables = std::get<1>(game_map);
		auto game_map_tiles = std::get<2>(game_map);

		for (int i = 0; i < game_map_pre_renders.size(); i++)
		{
			auto map_tile_entity = gCoordinator->CreateEntity();
			Pre_Renderable* pre_render = new Pre_Renderable(game_map_pre_renders.at(i));
			gCoordinator->AddComponent(map_tile_entity, pre_render);
			Collisionable* collisionable = new Collisionable(game_map_collisionables.at(i));
			gCoordinator->AddComponent(map_tile_entity, collisionable);
			Tile* tile = new Tile(game_map_tiles.at(i));
			gCoordinator->AddComponent(map_tile_entity, tile);

			gCoordinator->AddComponent(map_tile_entity, Create_Gamestate_Component_from_Enum(ComponentTypeEnum::MAIN_GAME_STATE));
		}

		Entity test_person = Entity_Builder_Utils::Create_Person(*gCoordinator, ComponentTypeEnum::MAIN_GAME_STATE, 0, 0, "APOCALYPSE_MAP", "GUNMAN_1", true);
	

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

}