#pragma once

#include <ECS/Core/Coordinator.hpp>
#include <Constants.h>
#include <filesystem>
#include <ECS/Components/Components.hpp>

namespace SXNGN {
	namespace ECS {
			class Entity_Builder_Utils
			{
			public:
				static Entity Create_Tile(Coordinator coordinator, ComponentTypeEnum game_state, Sint32 x_grid, Sint32 y_grid, std::string sprite_sheet, std::string sprite_name, CollisionType collision_type, std::string name = "unnamed_tile");

				static Entity Create_Person(Coordinator coordinator, ComponentTypeEnum game_state, Sint32 x_grid, Sint32 y_grid, std::string sprite_sheet, std::string sprite_name, bool player_controlled = false, std::string name = "unnamed_person");

				static Entity Create_Event(Coordinator coordinator, ComponentTypeEnum game_state, Event_Component trigger_event, std::string name = "unnamed_person");

				static Entity Create_Mouse_Wheel_Event(Coordinator coordinator, ComponentTypeEnum game_state, int x, int y);

				static Entity Create_Mouse_Event(Coordinator coordinator, ComponentTypeEnum game_state, Click click, bool shift_click, bool alt_click, bool ctrl_click);

				static Entity Create_Selection_Event(Coordinator coordinator, ComponentTypeEnum game_state, std::vector<Entity> clicked, std::vector<Entity> dclicked, std::vector<Entity> boxed, bool additive, bool subtractive, bool enqueue);

				static Entity Create_Order_Event(Coordinator coordinator, ComponentTypeEnum game_state, OrderType order_type, std::vector<Entity> clicked, std::vector<Entity> dclicked, std::vector<Entity> boxed, bool additive, bool subtractive, bool enqueue);

				static Entity Create_Spawn_Event(Coordinator coordinator, ComponentTypeEnum game_state, int pixel_x, int pixel_y);

				//static Pre_Renderable* Create_Pre_Renderable(Sint32 x, Sint32 y, std::string sprite_sheet, std::string sprite_name, RenderLayer render_layer);

				//static Moveable* Create_Moveable(double pos_x_m, double pos_y_m, Sint32 speed_m_s = 7, MoveableType movement_type = MoveableType::VELOCITY);

				//static Collisionable* Create_Collisionable(SDL_Rect collision_box,CollisionType collision_type, CollisionTag collision_tag = CollisionTag::UNKNOWN);

				//static Tile* Create_Tile(Sint32 x_grid, Sint32 y_grid);

			};
		}
	}
