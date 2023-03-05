#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>
#include <nlohmann/json.hpp>
using nlohmann::json;

using ComponentTypeEnum = SXNGN::ECS::ComponentTypeEnum;

namespace SXNGN::ECS {


	/// <summary>
	/// Contains UUID of an entity
	/// </summary>
	struct UUID : ECS_Component
	{
		UUID()
		{
			uuid_ = sole::uuid1();
		}

		UUID(std::string str_init = "00000000-0000-0000-0000-000000000000")
		{
			uuid_ = sole::rebuild("00000000-0000-0000-0000-000000000000");
		}
		uuid  uuid_;
	};

	inline void to_json(json& j, const UUID& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::UUID]},
			{"uuid_", p.uuid_},
		};

	}

	inline void from_json(const json& j, UUID& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		j.at("uuid_").get_to(p.grid_x_);
	}


}