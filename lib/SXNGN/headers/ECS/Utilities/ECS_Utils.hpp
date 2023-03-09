#pragma once

#include <ECS/Core/Coordinator.hpp>
#include <Constants.h>
#include <filesystem>
#include <ECS/Components/Components.hpp>
#include <ECS/Core/Coordinator.hpp>
#include <ECS/Components/CameraComponent.hpp>
using ECS_Camera = SXNGN::ECS::CameraComponent;
namespace SXNGN {
	namespace ECS {
			class ECS_Utils
			{
			public:
				
				//static void ChangeEntityPosition(Entity entity, double x, double y);

				//static void ChangeEntityPositionLastGood(Entity entity);

				//static Coordinate GetEntityPixelCoordinate(Entity entity);

				static bool object_in_view(std::shared_ptr<ECS_Camera> camera, SDL_FRect object);

				static SDL_FRect determine_camera_lens_scaled(std::shared_ptr<ECS_Camera> camera);

				static SDL_FRect determine_camera_lens_unscaled(std::shared_ptr<ECS_Camera> camera);

				// Convert a position on the screen where (0,0) = top left to a world position where top left = camera top left + zoom in /scale shenanigans
				static SDL_FRect convert_screen_position_to_world_position(std::shared_ptr<ECS_Camera> camera, int entity_x_unscaled, int entity_y_unscaled, ComponentTypeEnum game_state = ComponentTypeEnum::MAIN_GAME_STATE);

				static void pause_game();

				static void unpause_game();

				static void send_event(ECS_Component* event_component);

			};
		}
	}