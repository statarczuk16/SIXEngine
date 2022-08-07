#pragma once

#include <SDL.h>
#include <vector>

namespace SXNGN {  
	class Tile;

	namespace ECS { namespace A {
		struct Collisionable;
	} } 
	

	class CollisionChecks
	{
	public:
		//Box collision detector
		static bool checkCollision(SDL_FRect a, SDL_FRect b);
		//Use buffer
		static bool checkCollisionRectRectBuffer(SDL_FRect a, SDL_FRect b, int buffer);

		static bool checkCollisionRectCircleBuffer(SDL_FRect a, SDL_FRect b, int buffer);

		static bool checkCollisionCircleCircleBuffer(SDL_FRect a, SDL_FRect b, int buffer);

		static SDL_FRect getCollisionLocation(SDL_FRect fixed, SDL_FRect moveable_now, SDL_FRect moveable_previous);

		static SDL_FRect getCollisionLocation(SDL_FRect fixed, SDL_FRect moveable);

	};

}

