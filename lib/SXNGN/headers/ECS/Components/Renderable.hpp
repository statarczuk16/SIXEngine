#pragma once

#include "SDL.h"
#include <Sprite/Tile.h>
#include <memory>

namespace SXNGN::ECS::Components {

	/// <summary>
	/// Contains all data needed to render a BASE_TILE_WIDTH*BASE_TILE_HEIGHT sprite
	/// </summary>
	struct Renderable
	{
		std::shared_ptr<SDL_Rect> bounding_box_;
		std::shared_ptr<SDL_Rect> tile_map_snip_;
		std::string tile_name_;
		std::shared_ptr<SXNGN::Texture> tile_texture_;
	};

	/// <summary>
	/// Contains all the data needed to tell the Renderable_Creator_System how to create a Renderable from this
	/// </summary>
	struct Pre_Renderable
	{
		std::string sprite_factory_name;//path to the sprite sheet to create from
		std::string sprite_factory_sprite_type;//name of the sprite to use
	};

	/// <summary>
	/// Renderable_Creator_System uses this + Pre_Renderable to create a Renderable
	/// </summary>
	struct Sprite_Factory
	{
		std::string sprite_factory_name;
		//Used to read manifest
		const std::map<std::string, SXNGN::TileType> manifest_string_to_type_map_ = { {"TILE_HEIGHT",TileType::META_HEIGHT},{"TILE_WIDTH",TileType::META_WIDTH},{"TILE_UNIT",TileType::UNIT},{"TILE_TERRAIN",TileType::TERRAIN} };

		//From manifest, the width and height of tiles in the sprite sheet (pixels)
		size_t tile_width_, tile_height_;

		//go from a map file to a string
		std::map<int, std::string> tile_name_int_to_string_map_;
		//where that string can go to a rect that is a tilesheet clip coordinate -- also used by Renderable_Creator_System to create a sprite from a name
		std::map< std::string, std::shared_ptr<SDL_Rect>> tile_name_string_to_rect_map_;
		//go from a tilename "SAND, WATER" etc to a TileType that contains collision or other data "WALL" etc
		std::map<std::string, SXNGN::TileType> tile_name_string_to_tile_type_map_;

	};

	/// <summary>
	/// Contains all the data needed by the Sprite_Factory_Creator_System to create a Sprite_Factory
	/// </summary>
	struct Pre_Sprite_Factory
	{
		//The name of the factory to create. Used for any time a sprite is to be created from that factory.
		std::string name_;
		//Path to manifest with data about the sprite sheet
		std::string tile_manifest_path_;

	};


}