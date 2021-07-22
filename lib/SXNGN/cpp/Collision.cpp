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

	 SDL_Rect SXNGN::CollisionChecks::getCollisionLocation(SDL_Rect fixed, SDL_Rect moveable_now, SDL_Rect moveable_prev)
	{
		 SDL_Rect new_move_location;
		 //The sides of the rectangles
		 int leftFix, leftMoveNow, leftMovePrev;
		 int rightFix, rightMoveNow, rightMovePrev;
		 int topFix, topMoveNow, topMovePrev;
		 int bottomFix, bottomMoveNow, bottomMovePrev;

		 //Calculate the sides of rect A
		 leftFix = fixed.x;
		 rightFix = fixed.x + fixed.w;
		 topFix = fixed.y;
		 bottomFix = fixed.y + fixed.h;

		 //Calculate the sides of rect B now
		 leftMoveNow = moveable_now.x;
		 rightMoveNow = moveable_now.x + moveable_now.w;
		 topMoveNow = moveable_now.y;
		 bottomMoveNow = moveable_now.y + moveable_now.h;

		 //determine if object is now colliding with static object
		 int xon = std::max(0, std::min(rightFix, rightMoveNow) - std::max(leftFix, leftMoveNow)) > 0;
		 int yon = std::max(0, std::min(bottomFix, bottomMoveNow) - std::max(topFix, topMoveNow)) > 0;
		 bool x_overlap_now = xon > 0;
		 bool y_overlap_now = yon > 0;
		 if (!(x_overlap_now > 0 && y_overlap_now > 0))
		 {
			 //if no collision, stop
			 new_move_location.x = moveable_now.x;
			 new_move_location.y = moveable_now.y;
			 return new_move_location;
		 }

		 //Calculate the sides of rect B previous
		 leftMovePrev = moveable_prev.x;
		 rightMovePrev = moveable_prev.x + moveable_prev.w;
		 topMovePrev = moveable_prev.y;
		 bottomMovePrev = moveable_prev.y + moveable_prev.h;

		 //determine if object WAS colliding before now
		 int xop = std::max(0, std::min(rightFix, rightMovePrev) - std::max(leftFix, leftMovePrev)) > 0;
		 int yop = std::max(0, std::min(bottomFix, bottomMovePrev) - std::max(topFix, topMovePrev)) > 0;
		 bool x_overlap_prev = xop > 0;
		 bool y_overlap_prev = yop > 0;

		 //collision hitting the side of the static - resolve to left or right side of fixed 
		 if (!x_overlap_prev && x_overlap_now)
		 {
			 if (moveable_prev.x < fixed.x)
			 {
				 new_move_location.x = leftFix - moveable_prev.w;
			 }
			 else
			 {
				 new_move_location.x = rightFix;
			 }
		}
		 else
		 {
			 new_move_location.x = moveable_now.x;
		 }
		 //collision hitting top or bottom of static - resolve to top or bottom of fixed
		 if (!y_overlap_prev && y_overlap_now)
		 {
			 if (moveable_prev.y < fixed.y)
			 {
				 new_move_location.y = topFix - moveable_prev.h;
			 }
			 else
			 {
				 new_move_location.y = bottomFix;
			 }
		 }
		 else
		 {
			 new_move_location.y = moveable_now.y;
		 }


		// printf("xon: %d yon: %d xop: %d yop: %d\n", xon, yon, xop, yop);
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