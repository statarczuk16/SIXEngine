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
		Tile()
		{
			component_type = ComponentTypeEnum::TILE;
		}

	};

	inline void to_json(json& j, const Tile& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::TILE]},

		};

	}

	inline void from_json(const json& j, Tile& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;


	}


}