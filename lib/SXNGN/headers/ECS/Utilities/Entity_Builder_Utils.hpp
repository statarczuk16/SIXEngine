#pragma once

#include <ECS/Core/Coordinator.hpp>
#include <Constants.h>
#include <filesystem>
#include <ECS/Components/Components.hpp>
#include <ECS/Core/Coordinator.hpp>
namespace SXNGN {
	namespace ECS {
		class Entity_Builder_Utils
		{
		public:
			static Entity Create_Tile(A::Coordinator coordinator, Sint32 x_grid, Sint32 y_grid, std::string sprite_sheet, std::string sprite_name, A::CollisionType collision_type, std::string name = "unnamed_tile");

			static Entity Create_Person(A::Coordinator coordinator, Sint32 x_grid, Sint32 y_grid, std::string sprite_sheet, std::string sprite_name, bool player_controlled = false, std::string name = "unnamed_person");

			//static A::Pre_Renderable* Create_Pre_Renderable(Sint32 x, Sint32 y, std::string sprite_sheet, std::string sprite_name, A::RenderLayer render_layer);

			static A::Moveable* Create_Moveable(double pos_x_m, double pos_y_m, Sint32 speed_m_s = 7, A::MoveableType movement_type = A::MoveableType::VELOCITY);

			static A::Collisionable* Create_Collisionable(SDL_Rect collision_box, A::CollisionType collision_type);

			static A::Tile* Create_Tile(Sint32 x_grid, Sint32 y_grid);
		};
	}
}
