#pragma once

#include <ECS/Core/Coordinator.hpp>
#include <Constants.h>
#include <filesystem>
#include <ECS/Components/Components.hpp>
#include <ECS/Core/Coordinator.hpp>
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
			};
		}
	}
}