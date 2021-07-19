#pragma once

#include <ECS/Core/Coordinator.hpp>
#include <Constants.h>
#include <filesystem>
#include <ECS/Components/Components.hpp>
#include <ECS/Core/Coordinator.hpp>
#include <ECS/Components/CameraComponent.hpp>
using ECS_Camera = SXNGN::ECS::A::CameraComponent;
namespace SXNGN {
	namespace ECS {
		namespace A {
			class ECS_Utils
			{
			public:
				//change entity position to input. if confident, will update the prev or "last known good" position.
				static void ChangeEntityPosition(Entity entity, double x, double y, bool confident);

				static void ChangeEntityPositionLastGood(Entity entity);

				static std::shared_ptr<SDL_Rect> GetEntityPosition(Entity entity);

				static bool object_in_view(std::shared_ptr<ECS_Camera> camera, SDL_Rect object);

				static SDL_Rect determine_camera_lens_scaled(std::shared_ptr<ECS_Camera> camera);

				static SDL_Rect determine_camera_lens_unscaled(std::shared_ptr<ECS_Camera> camera);

				// Convert a position on the screen where (0,0) = top left to a world position where top left = camera top left + zoom in /scale shenanigans
				static SDL_Rect convert_screen_position_to_world_position(std::shared_ptr<ECS_Camera> camera, int entity_x_unscaled, int entity_y_unscaled);
			};
		}
	}
}