#include "ECS/Systems/RenderSystem.hpp"

#include "ECS/Components/Camera.hpp"
#include "ECS/Components/Renderable.hpp"
#include "ECS/Components/Transform.hpp"
#include "ECS/Core/Coordinator.hpp"

#include <Database.h>

#include <cmath>
#include <fstream>


extern Coordinator gCoordinator;
using namespace SXNGN;

void Sprite_Factory_Creator_System::Init()
{
	

}


void Sprite_Factory_Creator_System::Update(float dt)
{
	for (auto const& entity : mEntities)
	{

		//go from a map file to a string
		std::map<int, std::string> tile_name_int_to_string_map;
		//where that string can go to a rect that is a tilesheet clip coordinate
		std::map< std::string, std::shared_ptr<SDL_Rect>> tile_name_string_to_rect_map;

		size_t  tile_width, tile_height;

		const std::map<std::string, SXNGN::TileType> manifest_string_to_type_map_ = { {"TILE_HEIGHT",TileType::META_HEIGHT},{"TILE_WIDTH",TileType::META_WIDTH},{"TILE_UNIT",TileType::UNIT},{"TILE_TERRAIN",TileType::TERRAIN} };


		auto& pre_factory = gCoordinator.GetComponent<SXNGN::ECS::Components::Pre_Sprite_Factory>(entity);

		std::string tile_manifest_path_ = pre_factory.tile_manifest_path_;

		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Sprite_Factory_Creator_System::Creating Sprite_Factory from ", tile_manifest_path_.c_str());

		std::ifstream in(tile_manifest_path_);

		if (!in)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Sprite_Factory_Creator_System::Fatal: Cannot open tile manifest : % s", tile_manifest_path_.c_str());
			Database::get_coordinator()->DestroyEntity(entity);
			return;
		}
		//Open the map
		std::string str;
		int tile_name_idx = 0;
		char delimiter = ',';
		std::string w = "";
		std::vector < std::vector< std::string> > manifest_vector;
		std::vector< std::string>  line_split_results;
		while (std::getline(in, str))
		{
			line_split_results.clear();
			w = "";
			for (auto rem : str)
			{
				if (rem == delimiter)
				{
					if (w != "")
					{
						line_split_results.push_back(w);
					}

					w = "";
				}
				else
				{
					w = w + rem;
				}

			}
			if (w != "")
			{
				line_split_results.push_back(w);
			}
			manifest_vector.push_back(line_split_results);
		}

		
		for (auto manifest_entry : manifest_vector)
		{

			if (manifest_entry.empty())
			{
				//std::cout << "Empty line..." << std::endl;
				continue;
			}
			std::string manifest_entry_type_str = manifest_entry.at(0);
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, " Read Type: ", manifest_entry_type_str.c_str());
			SXNGN::TileType manifest_tile_type;
			if (manifest_string_to_type_map_.count(manifest_entry_type_str) > 0)
			{
				manifest_tile_type = manifest_string_to_type_map_.at(manifest_entry_type_str);
			}
			else
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Uknown manifest entry type: ", manifest_entry_type_str.c_str());
				continue;
			}

			switch (manifest_tile_type)
			{
			case TileType::META_HEIGHT:
			{
				if (manifest_entry.size() == 2)
				{
					tile_height = std::stoi(manifest_entry.at(1));
				}
				else
				{
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error loading TileManifest: META_HEIGHT");
				}
				break;
			}
			case TileType::META_WIDTH:
			{
				if (manifest_entry.size() == 2)
				{
					tile_width = std::stoi(manifest_entry.at(1));
				}
				else
				{
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error loading TileManifest: META_WIDTH");
				}
				break;
			}
			case TileType::TERRAIN:
			{
				if (manifest_entry.size() == 5)
				{
					std::shared_ptr<SDL_Rect> entry = std::make_shared<SDL_Rect>();
					int x_grid = std::stoi(manifest_entry.at(1));
					int y_grid = std::stoi(manifest_entry.at(2));
					std::string tile_name = manifest_entry.at(3);
					int int_to_name = std::stoi(manifest_entry.at(4));
					entry->x = x_grid * tile_width;
					entry->y = y_grid * tile_height;
					entry->w = tile_width;
					entry->h = tile_height;
					tile_name_string_to_rect_map[tile_name] = entry;
					tile_name_int_to_string_map[int_to_name] = tile_name;
				}
				else
				{
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error loading TileManifest: TERRAIN");
				}
				break;
			}
			case TileType::UNIT:
			{
				if (manifest_entry.size() == 4)
				{
					std::shared_ptr<SDL_Rect> entry = std::make_shared<SDL_Rect>();
					int x_grid = std::stoi(manifest_entry.at(1));
					int y_grid = std::stoi(manifest_entry.at(2));
					std::string tile_name = manifest_entry.at(3);
					entry->x = x_grid * tile_width;
					entry->y = y_grid * tile_height;
					entry->w = tile_width;
					entry->h = tile_height;
					tile_name_string_to_rect_map[tile_name] = entry;
				}
				else
				{
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error loading TileManifest: UNIT");
				}
				break;
			}

			}
		}



		in.close();
		Database::get_coordinator()->DestroyEntity(entity);

		auto sprite_factory_entity = Database::get_coordinator()->CreateEntity();
		SXNGN::ECS::Components::Sprite_Factory sprite_factory_component;
		sprite_factory_component.sprite_factory_name = pre_factory.name_,
			sprite_factory_component.tile_height_ = tile_height;
		sprite_factory_component.tile_width_ = tile_width;
		sprite_factory_component.tile_name_int_to_string_map_ = tile_name_int_to_string_map;
		sprite_factory_component.tile_name_string_to_rect_map_ = tile_name_string_to_rect_map;

		Database::get_coordinator()->AddComponent(sprite_factory, SXNGN::ECS::Components::Sprite_Factory(
			pre_factory.name_,


			);

		
	}

	
}
