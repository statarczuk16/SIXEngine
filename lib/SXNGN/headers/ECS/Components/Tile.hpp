#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>
#include <nlohmann/json.hpp>
using nlohmann::json;

using ComponentTypeEnum = SXNGN::ECS::A::ComponentTypeEnum;

namespace SXNGN::ECS::A {


	/// <summary>
	/// Contains data about a ground level tile
	/// </summary>
	struct Tile : ECS_Component
	{
		Tile(Sint32 grid_x, Sint32 grid_y)
		{
			grid_y_ = grid_y;
			grid_x_ = grid_x;
			component_type = ComponentTypeEnum::TILE;

		}

		Tile()
		{
			grid_y_ = 0;
			grid_x_ = 0;
			component_type = ComponentTypeEnum::TILE;
		}
		Sint32  grid_x_;
		Sint32  grid_y_;
	};

	inline void to_json(json& j, const Tile& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::TILE]},
			{"grid_x_", p.grid_x_},
			{"grid_y_", p.grid_y_},
		};

	}

	inline void from_json(const json& j, Tile& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		j.at("grid_x_").get_to(p.grid_x_);
		j.at("grid_y_").get_to(p.grid_y_);

	}


}