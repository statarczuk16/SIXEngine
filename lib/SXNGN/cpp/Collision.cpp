#include <Collision.h>
#include <ECS/Components/Collision.hpp>



namespace SXNGN {

	


	bool SXNGN::CollisionChecks::checkCollision(SDL_FRect a, SDL_FRect b)
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

	bool SXNGN::CollisionChecks::checkCollisionRectRectBuffer(SDL_FRect a, SDL_FRect b, int buffer)
	{
		//The sides of the rectangles
		float leftA, leftB;
		float rightA, rightB;
		float topA, topB;
		float bottomA, bottomB;

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
		if ((bottomA - topB) + buffer <= 0)
		{
			return false;
		}

		if ((topA - bottomB) - buffer >= 0)
		{
			return false;
		}

		if ((rightA - leftB) + buffer <= 0)
		{
			return false;
		}

		if ((leftA - rightB) - buffer >= 0)
		{
			return false;
		}

		//If none of the sides from A are outside B
		return true;
	}

	bool SXNGN::CollisionChecks::checkCollisionRectCircleBuffer(SDL_FRect rect, SDL_FRect circle, int buffer)
	{

		float xB;
		float yB;
		double radiusB;

		xB = circle.x;
		yB = circle.y;
		radiusB = circle.w;
		

		double nX = std::max(rect.x, std::min(rect.x + rect.w, xB));
		double nY = std::max(rect.y, std::min(rect.y + rect.h, yB));

		double magnitude = sqrt(pow((nX - xB), 2.0) + pow((nY - yB),2.0));

		if (magnitude <= radiusB)
		{
			return true;
		}
		return false;
	}

	bool SXNGN::CollisionChecks::checkCollisionCircleCircleBuffer(SDL_FRect a, SDL_FRect b, int buffer)
	{

		return false;
	}

	SDL_FRect SXNGN::CollisionChecks::getCollisionLocation(SDL_FRect fixed, SDL_FRect moveable)
	{
		SDL_FRect new_move_location;
		//The sides of the rectangles
		float leftFix, leftMoveNow, leftMovePrev;
		float rightFix, rightMoveNow, rightMovePrev;
		float topFix, topMoveNow, topMovePrev;
		float bottomFix, bottomMoveNow, bottomMovePrev;

		//Calculate the sides of rect A
		leftFix = fixed.x;
		rightFix = fixed.x + fixed.w;
		topFix = fixed.y;
		bottomFix = fixed.y + fixed.h;

		//Calculate the sides of rect B now
		leftMoveNow = moveable.x;
		rightMoveNow = moveable.x + moveable.w;
		topMoveNow = moveable.y;
		bottomMoveNow = moveable.y + moveable.h;

		//determine if object is now colliding with static object
		float xon = std::max(float(0.0), std::min(rightFix, rightMoveNow) - std::max(leftFix, leftMoveNow)) > 0.0;
		float yon = std::max(float(0.0), std::min(bottomFix, bottomMoveNow) - std::max(topFix, topMoveNow)) > 0.0;
		bool x_overlap_now = xon > 0;
		bool y_overlap_now = yon > 0;
		if ((x_overlap_now && y_overlap_now) == false)
		{
			//if no collision, stop
			new_move_location.x = moveable.x;
			new_move_location.y = moveable.y;
			return new_move_location;
		}

		//collision hitting the side of the static - resolve to left or right side of fixed 
		if (x_overlap_now)
		{
			//if overlappying position is closer to left side
			if (moveable.x < (fixed.x + (fixed.w / 2)))
			{
				new_move_location.x = leftFix - moveable.w;
			}
			else
			{
				new_move_location.x = rightFix;
			}
			return new_move_location;
		}
		else
		{
			new_move_location.x = moveable.x;
		}
		//collision hitting top or bottom of static - resolve to top or bottom of fixed
		if (y_overlap_now)
		{
			if (moveable.y < (fixed.y + (fixed.h / 2)))
			{
				new_move_location.y = topFix - moveable.h;
			}
			else
			{
				new_move_location.y = bottomFix;
			}
			return new_move_location;
		}
		else
		{
			new_move_location.y = moveable.y;
		}


		// printf("xon: %d yon: %d xop: %d yop: %d\n", xon, yon, xop, yop);
		return new_move_location;


	}

	 SDL_FRect SXNGN::CollisionChecks::getCollisionLocation(SDL_FRect fixed, SDL_FRect moveable_now, SDL_FRect moveable_prev)
	{
		 SDL_FRect new_move_location;
		 //The sides of the rectangles
		 float leftFix, leftMoveNow, leftMovePrev;
		 float rightFix, rightMoveNow, rightMovePrev;
		 float topFix, topMoveNow, topMovePrev;
		 float bottomFix, bottomMoveNow, bottomMovePrev;

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
		 int xon = std::max(float(0), (std::min)(rightFix, rightMoveNow) - (std::max)(leftFix, leftMoveNow)) > 0;
		 int yon = std::max(float(0), (std::min)(bottomFix, bottomMoveNow) - (std::max)(topFix, topMoveNow)) > 0;
		 bool x_overlap_now = xon > 0;
		 bool y_overlap_now = yon > 0;
		 if ((x_overlap_now && y_overlap_now) == false)
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
		 int xop = (std::max)(float(0), (std::min)(rightFix, rightMovePrev) - (std::max)(leftFix, leftMovePrev)) > 0;
		 int yop = (std::max)(float(0), (std::min)(bottomFix, bottomMovePrev) - (std::max)(topFix, topMovePrev)) > 0;
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



}