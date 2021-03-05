#include <Collision.h>
#include <Sprite/Tile.h>
#include <ECS/Components/Collision.hpp>

namespace SXNGN {

	bool SXNGN::CollisionChecks::checkCollision(SDL_Rect a, SDL_Rect b)
	{
		//The sides of the rectangles
		int leftA, leftB;
		int rightA, rightB;
		int topA, topB;
		int bottomA, bottomB;

		//Calculate the sides of rect A
		leftA = a.x;
		rightA = a.x + a.w;
		topA = a.y;
		bottomA = a.y + a.h;

		//Calculate the sides of rect B
		leftB = b.x;
		rightB = b.x + b.w;
		topB = b.y;
		bottomB = b.y + b.h;

		//If any of the sides from A are outside of B
		if (bottomA <= topB)
		{
			return false;
		}

		if (topA >= bottomB)
		{
			return false;
		}

		if (rightA <= leftB)
		{
			return false;
		}

		if (leftA >= rightB)
		{
			return false;
		}

		//If none of the sides from A are outside B
		return true;
	}

	bool SXNGN::CollisionChecks::checkCollisionBuffer(SDL_Rect a, SDL_Rect b, int buffer)
	{
		//The sides of the rectangles
		int leftA, leftB;
		int rightA, rightB;
		int topA, topB;
		int bottomA, bottomB;

		//Calculate the sides of rect A
		leftA = a.x;
		rightA = a.x + a.w;
		topA = a.y;
		bottomA = a.y + a.h;

		//Calculate the sides of rect B
		leftB = b.x;
		rightB = b.x + b.w;
		topB = b.y;
		bottomB = b.y + b.h;

		//If any of the sides from A are outside of B
		if ((bottomA - topB) + buffer < 0)
		{
			return false;
		}

		if ((topA - bottomB) - buffer > 0)
		{
			return false;
		}

		if ((rightA - leftB) + buffer < 0)
		{
			return false;
		}

		if ((leftA - rightB) - buffer > 0)
		{
			return false;
		}

		//If none of the sides from A are outside B
		return true;
	}


	//fixme should this be here?
	bool SXNGN::CollisionChecks::touchesWall(SDL_Rect box, std::vector<SXNGN::Tile> tiles)
	{
		//Go through the tiles
		for (int i = 0; i < tiles.size(); ++i)
		{
			//If the tile is a wall type tile
			if ((tiles.at(i).getType() == TileType::WALL))
			{
				//If the collision box touches the wall tile
				if (CollisionChecks::checkCollision(box, *tiles.at(i).getCollisionBox()))
				{
					return true;
				}
			}
		}

		//If no wall tiles were touched
		return false;
	}

	using CollisionType = SXNGN::ECS::Components::CollisionType;
	 std::vector<std::vector<const SXNGN::ECS::Components::CollisionComponent*>> CollisionChecks::determineCollisions(SDL_Rect box, std::vector<const SXNGN::ECS::Components::CollisionComponent*> collision_components, int buffer)
	{
		 std::vector<const SXNGN::ECS::Components::CollisionComponent*> immoveable_collisions;
		 std::vector<const SXNGN::ECS::Components::CollisionComponent*> elastic_collisions;

		for(int i = 0; i < collision_components.size(); i++)
		{
			if (checkCollisionBuffer(box, collision_components.at(i)->collision_box_, buffer))
			{
				switch (collision_components.at(i)->collision_type_)
				{
				case CollisionType::IMMOVEABLE:
				{
					immoveable_collisions.push_back(collision_components.at(i));
					break;
				}
				case CollisionType::ELASTIC:
				{
					elastic_collisions.push_back(collision_components.at(i));
					break;
				}
				}
			}
		}

		std::vector<std::vector<const SXNGN::ECS::Components::CollisionComponent*>> ret = { immoveable_collisions , elastic_collisions };
		return ret;
	}
}