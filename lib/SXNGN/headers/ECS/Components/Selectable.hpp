#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>
#include <nlohmann/json.hpp>
using nlohmann::json;

namespace SXNGN::ECS {


	/// <summary>
	/// Contains data displayed when selected
	/// </summary>
	struct Selectable : ECS_Component
	{
		Selectable()
		{
			component_type = ComponentTypeEnum::SELECTABLE;
		}
		std::string selection_text_ = "";
	};

	
	inline void to_json(json& j, const Selectable& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::SELECTABLE]},
			{"selection_text_", p.selection_text_},
		};

	}

	inline void from_json(const json& j, Selectable& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		j.at("selection_text_").get_to(p.selection_text_);
	}
}