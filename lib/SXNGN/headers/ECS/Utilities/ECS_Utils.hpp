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
				static void ChangeEntityPosition(Entity entity, double x, double y);

				static void ChangeEntityPositionLastGood(Entity entity);
			};
		}
	}
}