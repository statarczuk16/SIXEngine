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

	 SDL_Rect SXNGN::CollisionChecks::getCollisionLocation(SDL_Rect fixed, SDL_Rect moved)
	{
		 SDL_Rect new_move_location;
		 //The sides of the rectangles
		 int leftFix, leftMove;
		 int rightFix, rightMove;
		 int topFix, topMove;
		 int bottomFix, bottomMove;

		 //Calculate the sides of rect A
		 leftFix = fixed.x;
		 rightFix = fixed.x + fixed.w;
		 topFix = fixed.y;
		 bottomFix = fixed.y + fixed.h;

		 //Calculate the sides of rect B
		 leftMove = moved.x;
		 rightMove = moved.x + moved.w;
		 topMove = moved.y;
		 bottomMove = moved.y + moved.h;

		 int x_overlap = std::max(0, std::min(rightFix, rightMove) - std::max(leftFix, leftMove));
		 int y_overlap = std::max(0, std::min(bottomFix, bottomMove) - std::max(topFix, topMove));
		 if (!(x_overlap > 0 && y_overlap > 0))
		 {
			 new_move_location.x = moved.x;
			 new_move_location.y = moved.y;
			 return new_move_location;
		 }

		 if (x_overlap <= 0)
		 {
			 new_move_location.x = moved.x;
		 }
		 else
		 {		
			 //if the object that collides with the fixed object...
			 //to the right of the fixed object
			 //new position is the fixed object right side + moved object width
			 if (moved.x > fixed.x)
			 {
				 new_move_location.x = rightFix;
			 }
			 else
			 {
				 new_move_location.x = leftFix - moved.w;
			 }
		 }

		 if (y_overlap <= 0)
		 {
			 new_move_location.y = moved.y;
		 }
		 else
		 {
			 //if the object that collides with the fixed object...
			 //to below the fixed obj
			 //new position is the fixed object bottom + moved object height
			 if (moved.y > fixed.y)
			 {
				 new_move_location.y = bottomFix;
			 }
			 else
			 {
				 new_move_location.y = topFix - moved.h;
			 }
		 }

		 return new_move_location;
		
		 
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

	using CollisionType = SXNGN::ECS::A::CollisionType;
	 std::vector<std::vector<const SXNGN::ECS::A::Collisionable*>> CollisionChecks::determineCollisions(SDL_Rect box, std::vector<const SXNGN::ECS::A::Collisionable*> collision_components, int buffer)
	{
		 std::vector<const SXNGN::ECS::A::Collisionable*> immoveable_collisions;
		 std::vector<const SXNGN::ECS::A::Collisionable*> elastic_collisions;

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

		std::vector<std::vector<const SXNGN::ECS::A::Collisionable*>> ret = { immoveable_collisions , elastic_collisions };
		return ret;
	}
}