#pragma once

#include <SDL.h>

#include <memory>
#include <ECS/Core/Component.hpp>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <Texture.h>
#include <mutex>
#include <ECS/Components/Renderable.hpp>
using nlohmann::json;

using ComponentTypeEnum = SXNGN::ECS::ComponentTypeEnum;

namespace SXNGN::ECS {


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

		std::shared_ptr<Texture> sprite_map_texture_;
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
	
	/// <summary>
	/// Singleton used to store input state
	/// </summary>
	class SpriteFactoryHolder : ECS_Component
	{
	private:

		//CameraComponent::CameraComponent(const CameraComponent& source);

		//CameraComponent::CameraComponent(CameraComponent&& source);

		static SpriteFactoryHolder* instance_;
		static std::mutex lock_;

		SpriteFactoryHolder() {

		}

	public:
		static SpriteFactoryHolder* get_instance()
		{
			std::lock_guard<std::mutex> guard(lock_);//Wait until data is available (no other theadss have checked it out
			if (instance_ == nullptr)
			{
				instance_ = new SpriteFactoryHolder();
			}
			return instance_;
		}

		std::unordered_map<std::string, std::shared_ptr<Sprite_Factory>> sprite_factories_;
	};

	

}
