#pragma once

#include <ECS/Core/Coordinator.hpp>
#include <Constants.h>
#include <filesystem>
#include <ECS/Components/Components.hpp>
#include <ECS/Core/Coordinator.hpp>
namespace SXNGN {
	namespace ECS {
		namespace A {
			class Entity_Builder_Utils
			{
			public:
				static Entity Create_Tile(Coordinator coordinator, ComponentTypeEnum game_state, Sint32 x_grid, Sint32 y_grid, std::string sprite_sheet, std::string sprite_name, CollisionType collision_type, std::string name = "unnamed_tile");

				static Entity Create_Person(Coordinator coordinator, ComponentTypeEnum game_state, Sint32 x_grid, Sint32 y_grid, std::string sprite_sheet, std::string sprite_name, bool player_controlled = false, std::string name = "unnamed_person");

				//static A::Pre_Renderable* Create_Pre_Renderable(Sint32 x, Sint32 y, std::string sprite_sheet, std::string sprite_name, A::RenderLayer render_layer);

				static Moveable* Create_Moveable(double pos_x_m, double pos_y_m, Sint32 speed_m_s = 7, MoveableType movement_type = MoveableType::VELOCITY);

				static Collisionable* Create_Collisionable(SDL_Rect collision_box,CollisionType collision_type);

				static Tile* Create_Tile(Sint32 x_grid, Sint32 y_grid);

			};
		}
	}
}
