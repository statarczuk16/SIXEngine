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


		Tile(int traversal_cost = 1)
		{
			component_type = ComponentTypeEnum::TILE;
			traversal_cost_ = traversal_cost;
		}
		int traversal_cost_ = 0;

	};

	inline void to_json(json& j, const Tile& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::TILE]},
			{"traversal_cost_", p.traversal_cost_}
		};

	}

	inline void from_json(const json& j, Tile& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		j.at("traversal_cost_").get_to(p.traversal_cost_);


	}


}