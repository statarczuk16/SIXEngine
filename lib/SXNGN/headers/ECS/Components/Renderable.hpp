#pragma once

#include <SDL.h>

#include <memory>
#include <ECS/Core/Component.hpp>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <Texture.h>
using nlohmann::json;

using ComponentTypeEnum = SXNGN::ECS::A::ComponentTypeEnum;

namespace SXNGN::ECS::A {


	enum class RenderLayer
	{
		UNKNOWN,
		GROUND_LAYER,
		OBJECT_LAYER,
		AIR_LAYER,
		UI_LAYER
		
	};


	/// <summary>
	/// Contains all data needed to render a BASE_TILE_WIDTH*BASE_TILE_HEIGHT sprite
	/// </summary>
	struct Renderable : ECS_Component
	{
		Renderable(Sint32 x, Sint32 y, SDL_Rect tile_map_snip, std::string sprite_factory_name, std::string sprite_factory_sprite_type, std::shared_ptr<Texture> sprite_map_texture, A::RenderLayer render_layer, std::string renderable_name = "Nameless Renderable")
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

		Renderable()
		{
			component_type = ComponentTypeEnum::RENDERABLE;
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
		std::shared_ptr<Texture> sprite_map_texture_;
		RenderLayer render_layer_ = RenderLayer::UNKNOWN;
		std::string sprite_factory_name_;//path to the sprite sheet to create from
		std::string sprite_factory_sprite_type_;//name of the sprite to use
		bool outline = false;
	};

	//note: saving as a pre-renderable. Can't convert the texture pointer to JSON, so it will have to go back through the factory pre-render -> render to get back to its state
	inline void to_json(json& j, const Renderable& p) {		
		j = json{ 
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::PRE_RENDERABLE]},
			{"x", p.x_}, {"y", p.y_},
			{"sprite_factory_name_", p.sprite_factory_name_},
			{"sprite_factory_sprite_type_", p.sprite_factory_sprite_type_},
			{"name_", p.renderable_name_},
			{"render_layer_", p.render_layer_}
		};
		
	}

	inline void from_json(const json& j, Renderable& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		j.at("sprite_factory_name_").get_to(p.sprite_factory_name_);
		j.at("sprite_factory_sprite_type_").get_to(p.sprite_factory_sprite_type_);
		j.at("name_").get_to(p.renderable_name_);
		j.at("x").get_to(p.x_);
		j.at("y").get_to(p.y_);
		j.at("render_layer_").get_to(p.render_layer_);
	}

	/// <summary>
	/// Contains all the data needed to tell the Renderable_Creator_System how to create a Renderable from this
	/// </summary>
	struct Pre_Renderable : ECS_Component
	{
		Pre_Renderable(Sint32 x_in, Sint32 y_in, std::string sprite_sheet, std::string sprite_name, A::RenderLayer render_layer, std::string name = "Unnamed Renderable")
		{
			sprite_factory_name_ = sprite_sheet;
			sprite_factory_sprite_type_ = sprite_name;
			render_layer_ = render_layer;
			x = x_in;
			y = y_in;
			name_ = name;
			component_type = ComponentTypeEnum::PRE_RENDERABLE;
		}

		//Need default constructor for JSON
		Pre_Renderable()
		{
			component_type = ComponentTypeEnum::PRE_RENDERABLE;
		}

		std::string sprite_factory_name_;//path to the sprite sheet to create from
		std::string sprite_factory_sprite_type_;//name of the sprite to use
		std::string name_;
		Sint32 x, y;
		RenderLayer render_layer_ = RenderLayer::UNKNOWN;
	};


	inline void to_json(json& j, const Pre_Renderable& p) {
		j = json{ {"component_type",component_type_enum_to_string().at(p.component_type)}, {"x", p.x}, {"y", p.y}, {"sprite_factory_name_", p.sprite_factory_name_}, {"sprite_factory_sprite_type_", p.sprite_factory_sprite_type_},  {"name_", p.name_},  {"render_layer_", p.render_layer_} };
	}

	inline void from_json(const json& j, Pre_Renderable& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		j.at("sprite_factory_name_").get_to(p.sprite_factory_name_);
		j.at("sprite_factory_sprite_type_").get_to(p.sprite_factory_sprite_type_);
		j.at("name_").get_to(p.name_);
		j.at("x").get_to(p.x);
		j.at("y").get_to(p.y);
		j.at("render_layer_").get_to(p.render_layer_);

	}

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



}
