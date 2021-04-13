#pragma once

#include <SDL.h>
#include <vector>

namespace SXNGN {  
	class Tile;

	namespace ECS { namespace A {
		class Collisionable;
	} } 
	

	class CollisionChecks
	{
	public:
		//Box collision detector
		static bool checkCollision(SDL_Rect a, SDL_Rect b);
		//Use buffer
		static bool checkCollisionBuffer(SDL_Rect a, SDL_Rect b, int buffer);

		static SDL_Rect getCollisionLocation(SDL_Rect fixed, SDL_Rect moveable);

		//Checks collision box against set of tiles
		static bool touchesWall(SDL_Rect box, Tile* tiles[]);

		static bool touchesWall(SDL_Rect box, std::vector<SXNGN::Tile> tiles);

		/// <summary>
		/// Compared input bounding box with collisionables 
		/// Return vector of vectors of collisionables it collides with
		/// where each vector contains a different type of collision
		/// </summary>
		/// <param name="box"></param>
		/// <param name=""></param>
		/// <returns></returns>
		static std::vector<std::vector<const SXNGN::ECS::A::Collisionable*>>  determineCollisions(SDL_Rect box, std::vector<const SXNGN::ECS::A::Collisionable*>, int buffer);

		//static std::vector<std::vector<Collisionable>> determineCollisions(SDL_Rect box, std::vector<Collisionable> collision_components, int buffer);
	};

}

