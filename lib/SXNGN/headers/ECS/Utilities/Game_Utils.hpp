#pragma once

#include <ECS/Core/Coordinator.hpp>
#include <Constants.h>
#include <filesystem>
#include <ECS/Components/Components.hpp>
#include <ECS/Core/Coordinator.hpp>
namespace SXNGN {
	namespace ECS {
		namespace A {
			class Game_Utils
			{
				static std::tuple<std::vector<Pre_Renderable>, std::vector<Collisionable>, std::vector<Tile>>(int tile_chunks_width, int tile_chunks_height)
				{
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
							auto pre_render = new A::Pre_Renderable(x_pixels, y_pixels, default_tileset, default_tile, A::RenderLayer::GROUND_LAYER);
							auto collision = Entity_Builder_Utils::Create_Collisionable(collision_box, A::CollisionType::NONE);
							auto tile = Entity_Builder_Utils::Create_Tile(w, h);
							pre_renders.push_back(*pre_render);
							collisionables.push_back(*collision);
							tiles.push_back(*tile);
						}
					}
					return std::make_tuple(pre_renders, collisionables, tiles);
				}

				static int create_new_world()
				{
					auto gCoordinator = Database::get_coordinator();
					

					std::forward_list<ComponentTypeEnum> active_game_states;
					active_game_states.push_front(ComponentTypeEnum::MAIN_MENU_STATE);
					active_game_states.push_front(ComponentTypeEnum::CORE_BG_GAME_STATE);
					gCoordinator.GameStateChanged(active_game_states);

					Entity test_person = entity_builder::Create_Person(gCoordinator, ComponentTypeEnum::MAIN_GAME_STATE, 0, 0, "APOCALYPSE_MAP", "GUNMAN_1", true);
					
				}

				

			};
		}
	}
}