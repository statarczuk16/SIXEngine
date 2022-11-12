#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>
#include <nlohmann/json.hpp>
using nlohmann::json;


namespace SXNGN::ECS::A {


	/// <summary>
	/// Contains data displayed when selected
	/// </summary>
	struct WorldLocation : ECS_Component
	{
		WorldLocation()
		{
			component_type = ComponentTypeEnum::WORLD_LOCATION;
			location_name_ = "unremarkable desert";
			traversal_cost_ = 3;
			has_settlement_ = false;
			has_ruins_ = false;
			map_layer_ = RenderLayer::GROUND_LAYER;
		}
		std::string location_name_;
		int traversal_cost_;
		bool has_settlement_;
		bool has_ruins_;
		RenderLayer map_layer_;
		uint16_t map_grid_x_;
		uint16_t map_grid_y_;


	};

	inline void to_json(json& j, const WorldLocation& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::WORLD_LOCATION]},
			{"traversal_cost_", p.traversal_cost_},
			{"map_layer_", p.map_layer_},
			{"map_grid_x_", p.map_grid_x_},
			{"map_grid_y_", p.map_grid_y_}
		};
	}

		inline void from_json(const json & j, WorldLocation & p) {
			auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
			p.component_type = component_type_enum;
			j.at("traversal_cost_").get_to(p.traversal_cost_);
			j.at("map_layer_").get_to(p.map_layer_);
			j.at("map_grid_x_").get_to(p.map_grid_x_);
			j.at("map_grid_y_").get_to(p.map_grid_y_);
		};

	
	
	struct WorldMap : ECS_Component
	{
		WorldMap()
		{
			component_type = ComponentTypeEnum::WORLD_MAP;
		}
		std::vector< std::vector< std::vector< WorldLocation > > > world_locations_;
	};

	
	inline void to_json(json& j, const WorldMap& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::WORLD_MAP]},
			{"world_locations_", p.world_locations_},
		};

	}

	inline void from_json(const json& j, WorldMap& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		j.at("world_locations_").get_to(p.world_locations_);
	}
}