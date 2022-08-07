#pragma once

#include <Constants.h>
#include <ECS/Components/Components.hpp>
#include <climits>
namespace SXNGN {
	namespace ECS {
		namespace A {

			class Physics_Utils
			{
			public:

				static std::pair<float, float> CalculateDistanceTo(SDL_Rect e1, SDL_Rect e2);

				static std::pair<float, float> CalculateDistanceTo(SDL_FRect e1, SDL_FRect e2);

			};

		}
	}
}
