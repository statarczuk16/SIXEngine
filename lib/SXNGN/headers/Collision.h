#pragma once

#include <SDL.h>
#include <vector>

namespace SXNGN {  
	class Tile;

	namespace ECS { namespace Components {
		class CollisionComponent;
	} } 
	

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

		/// <summary>
		/// Compared input bounding box with collisionables 
		/// Return vector of vectors of collisionables it collides with
		/// where each vector contains a different type of collision
		/// </summary>
		/// <param name="box"></param>
		/// <param name=""></param>
		/// <returns></returns>
		static std::vector<std::vector<const SXNGN::ECS::Components::CollisionComponent*>>  determineCollisions(SDL_Rect box, std::vector<const SXNGN::ECS::Components::CollisionComponent*>, int buffer);

		//static std::vector<std::vector<CollisionComponent>> determineCollisions(SDL_Rect box, std::vector<CollisionComponent> collision_components, int buffer);
	};

}

