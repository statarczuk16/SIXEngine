#pragma once

#include <SDL.h>
#include <vector>

namespace SXNGN {  
	class Tile;

	class CollisionChecks
	{
	public:
		//Box collision detector
		static bool checkCollision(SDL_Rect a, SDL_Rect b);
		//Use > instead of >= (One pixel worth of grace area)
		static bool checkCollisionBuffer(SDL_Rect a, SDL_Rect b, int buffer);

		//Checks collision box against set of tiles
		static bool touchesWall(SDL_Rect box, Tile* tiles[]);

		static bool touchesWall(SDL_Rect box, std::vector<SXNGN::Tile> tiles);
	};

}

