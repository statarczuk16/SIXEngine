#pragma once

#include <SDL.h>
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

	enum class RenderLayer
	{
		UNKNOWN,
		GROUND_LAYER,
		OBJECT_LAYER,
		AIR_LAYER,
		UI_LAYER
		
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
		Renderable(Sint32 x, Sint32 y, SDL_Rect tile_map_snip, std::string sprite_factory_name, std::string sprite_factory_sprite_type, std::shared_ptr<SXNGN::Texture> sprite_map_texture, Components::RenderLayer render_layer, std::string renderable_name = "Nameless Renderable")
		{
			component_type = ComponentTypeEnum::RENDERABLE;
			x_ = x;
			y_ = y;
			tile_map_snip_ = tile_map_snip;
			renderable_name_ = renderable_name;
			sprite_map_texture_ = sprite_map_texture;
			render_layer_ = render_layer;
			sprite_factory_name_ = sprite_factory_name;
			sprite_factory_sprite_type_ = sprite_factory_sprite_type;
		}

		SDL_Rect get_bounding_box()
		{
			SDL_Rect box;
			box.x = x_;
			box.y = y_;
			box.w = tile_map_snip_.w;
			box.h = tile_map_snip_.h;
			return box;
		}
		//SDL_Rect bounding_box_;
		Sint32 x_, y_;
		SDL_Rect tile_map_snip_;
		std::string renderable_name_;
		std::shared_ptr<SXNGN::Texture> sprite_map_texture_;
		RenderLayer render_layer_ = RenderLayer::UNKNOWN;
		std::string sprite_factory_name_;//path to the sprite sheet to create from
		std::string sprite_factory_sprite_type_;//name of the sprite to use
	};

	/// <summary>
	/// Contains all the data needed to tell the Renderable_Creator_System how to create a Renderable from this
	/// </summary>
	struct Pre_Renderable : ECS_Component
	{
		Pre_Renderable(Sint32 x_in, Sint32 y_in, std::string sprite_sheet, std::string sprite_name, Components::RenderLayer render_layer, std::string name = "Unnamed Renderable")
		{
			sprite_factory_name_ = sprite_sheet;
			sprite_factory_sprite_type_ = sprite_name;
			render_layer_ = render_layer;
			x = x_in;
			y = y_in;
			name_ = name;
			component_type = ComponentTypeEnum::PRE_RENDERABLE;
		}
		std::string sprite_factory_name_;//path to the sprite sheet to create from
		std::string sprite_factory_sprite_type_;//name of the sprite to use
		std::string name_;
		Sint32 x, y;
		RenderLayer render_layer_ = RenderLayer::UNKNOWN;
	};

	

	/// <summary>
	/// Contains all data needed to render a collection
	/// </summary>
	struct Renderable_Batch : ECS_Component
	{
		Renderable_Batch()
		{
			component_type = ComponentTypeEnum::RENDERABLE_BATCH;
		}
		std::vector<Renderable> renderables;
	};



	/// <summary>
	/// Contains all the data needed to tell the Renderable_Creator_System how to create a Renderable from this
	/// </summary>
	struct Pre_Renderable_Batch : ECS_Component
	{
		Pre_Renderable_Batch()
		{
			component_type = ComponentTypeEnum::PRE_RENDERABLE_BATCH;
		}
		std::vector<Pre_Renderable> pre_renderables;
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

		std::string sprite_factory_name_;
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
