#pragma once

#include "SDL.h"
#include <Sprite/Tile.h>
#include <memory>
#include <ECS/Core/Component.hpp>
#include <unordered_map>

using ComponentTypeEnum = SXNGN::ECS::Components::ComponentTypeEnum;

namespace SXNGN::ECS::Components {


	enum class TileManifestType
	{
		NORMAL,
		WALL,
		UNIT,
		TERRAIN,
		META_HEIGHT,
		META_WIDTH,
		META_SPRITE_SHEET_SOURCE,
		UNKNOWN
	};

	//just a fancy way of making this a constant/accessible elsewhhere

	//map the text the sprite_factory_creator will read from manifest.txt to a useable enum
	inline std::unordered_map<std::string, TileManifestType>& manifest_string_to_type_map_() {
		static std::unordered_map<std::string, TileManifestType> manifest_string_to_type_map_ =
	{
		{"TILE_HEIGHT",TileManifestType::META_HEIGHT},
		{"TILE_WIDTH",TileManifestType::META_WIDTH},
		{"TILE_UNIT",TileManifestType::UNIT},
		{"TILE_TERRAIN",TileManifestType::TERRAIN} ,
		{"SPRITE_SHEET",TileManifestType::META_SPRITE_SHEET_SOURCE}
	};
	return manifest_string_to_type_map_;
	}
	


	/// <summary>
	/// Contains all data needed to render a BASE_TILE_WIDTH*BASE_TILE_HEIGHT sprite
	/// </summary>
	struct Renderable : ECS_Component
	{
		Renderable()
		{
			component_type = ComponentTypeEnum::RENDERABLE;
		}
		SDL_Rect bounding_box_;
		SDL_Rect tile_map_snip_;
		std::string tile_name_;
		std::shared_ptr<SXNGN::Texture> sprite_map_texture_;
	};

	/// <summary>
	/// Contains all the data needed to tell the Renderable_Creator_System how to create a Renderable from this
	/// </summary>
	struct Pre_Renderable : ECS_Component
	{
		Pre_Renderable()
		{
			component_type = ComponentTypeEnum::PRE_RENDERABLE;
			sprite_factory_name = SXNGN::BAD_STRING_RETURN;
			sprite_factory_sprite_type = SXNGN::BAD_STRING_RETURN;
			x = 0;
			y = 0;
		}
		std::string sprite_factory_name;//path to the sprite sheet to create from
		std::string sprite_factory_sprite_type;//name of the sprite to use
		size_t x, y;
	};

	/// <summary>
	/// Renderable_Creator_System uses this + Pre_Renderable to create a Renderable
	/// </summary>
	struct Sprite_Factory : ECS_Component
	{
		Sprite_Factory()
		{
			component_type = ComponentTypeEnum::SPRITE_FACTORY;
		}
		
		std::string sprite_factory_name;
		//From manifest, the width and height of tiles in the sprite sheet (pixels)
		size_t tile_width_, tile_height_;

		//go from a map file to a string
		std::map<int, std::string> tile_name_int_to_string_map_;
		//where that string can go to a rect that is a tilesheet clip coordinate -- also used by Renderable_Creator_System to create a sprite from a name
		std::map< std::string, std::shared_ptr<SDL_Rect>> tile_name_string_to_rect_map_;

		std::shared_ptr<SXNGN::Texture> sprite_map_texture_;


	};

	/// <summary>
	/// Contains all the data needed by the Sprite_Factory_Creator_System to create a Sprite_Factory
	/// </summary>
	struct Pre_Sprite_Factory : ECS_Component
	{
		Pre_Sprite_Factory()
		{
			component_type = ComponentTypeEnum::PRE_SPRITE_FACTORY;
		}
		//The name of the factory to create. Used for any time a sprite is to be created from that factory.
		std::string name_;
		//Path to manifest with data about the sprite sheet
		std::string tile_manifest_path_;

	};

}