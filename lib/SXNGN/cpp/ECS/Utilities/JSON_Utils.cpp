#include <ECS/Utilities/JSON_Utils.hpp>
#include <ECS/Utilities/Entity_Builder_Utils.hpp>
#include <iostream>


namespace SXNGN {
	namespace ECS {
		std::tuple<std::vector<Components::Pre_Renderable>, std::vector<Components::Collisionable>, std::vector<Components::Tile>>
			JSON_Utils::json_to_tile_batch(nlohmann::json jf)
		{
			int tiles_wide = 0;
			int tiles_high = 0;
			std::string default_tile = SXNGN::BAD_STRING_RETURN;
			std::string default_tileset = SXNGN::BAD_STRING_RETURN;
			printf("Reading tile_batch from JSON\n");
			for (auto& [key, val] : jf.items())
			{
				if (key == "tiles_width")
				{
					tiles_wide = val.get<int>();
				}
				else if (key == "tiles_height")
				{
					tiles_high = val.get<int>();
				}
				else if (key == "default_tileset")
				{
					default_tileset = val.get<std::string>();
				}
				else if (key == "default_tile")
				{
					default_tile = val.get<std::string>();
				}
				else
				{
					printf("!!!JSON_Utils::Error reading tile_batch with: \n");
					std::cout << "key: " << key << ", value:" << val << '\n';
					printf("!!! \n");
				}
			}

			printf("Preparing tile batch with:\n");
			printf("Tileset: %s\n",default_tileset.c_str());
			printf("Base tile: %s\n", default_tile.c_str());
			printf("Width: %d\n", tiles_wide);
			printf("Height: %d\n", tiles_high);

			std::vector<Components::Pre_Renderable> pre_renders;
			std::vector<Components::Collisionable> collisionables;
			std::vector<Components::Tile> tiles;

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
					auto pre_render = new Components::Pre_Renderable(x_pixels, y_pixels, default_tileset, default_tile, Components::RenderLayer::GROUND_LAYER);
					auto collision = Entity_Builder_Utils::Create_Collisionable(collision_box, Components::CollisionType::NONE);
					auto tile = Entity_Builder_Utils::Create_Tile(w, h);
					pre_renders.push_back(*pre_render);
					collisionables.push_back(*collision);
					tiles.push_back(*tile);
				}
			}
			return std::make_tuple(pre_renders, collisionables, tiles);
		}

		
	}
}