#pragma once

#include <SDL.h>

#include <memory>
#include <ECS/Core/Component.hpp>
#include <nlohmann/json.hpp>

using nlohmann::json;

using ComponentTypeEnum = SXNGN::ECS::A::ComponentTypeEnum;

namespace SXNGN::ECS::A {


	/// <summary>
	/// Contains all data aobut an entity's position
	/// </summary>
	struct Location : ECS_Component
	{
		Location(double x, double y)
		{
			m_pos_x_m_ = x;//current position x
			m_pos_y_m_ = y;//current position y
		}

		Location(int x, int y)
		{
			m_pos_x_m_ = (int)round(x);//current position x
			m_pos_y_m_ = (int)round(y);//current position y
		}

		Coordinate GetPixelCoordinate()
		{
			Coordinate coordinate(m_pos_x_m_, m_pos_y_m_);
			return coordinate;
		}

		Coordinate GetGridCoordinate()
		{
			Coordinate coordinate(m_pos_x_m_, m_pos_y_m_);
			coordinate.x / SXNGN::BASE_TILE_WIDTH;
			coordinate.x / SXNGN::BASE_TILE_HEIGHT;
			return coordinate;
		}

		double m_pos_x_m_ = 0.0;//current position x
		double m_pos_y_m_ = 0.0;//current position y


	};

	//note: saving as a pre-renderable. Can't convert the texture pointer to JSON, so it will have to go back through the factory pre-render -> render to get back to its state
	inline void to_json(json& j, const Location& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::LOCATION]},
			{"m_pos_x_m_", p.m_pos_x_m_}, {"m_pos_y_m_", p.m_pos_y_m_}
		};

	}

	inline void from_json(const json& j, Location& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		j.at("m_pos_x_m_").get_to(p.m_pos_x_m_);
		j.at("m_pos_y_m_").get_to(p.m_pos_y_m_);

	}
}

	