#include <Collision.h>
#include <Tile.h>


bool SXNGN::Collision::checkCollision(SDL_Rect a, SDL_Rect b)
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


//fixme should this be here?
 bool SXNGN::Collision::touchesWall(SDL_Rect box, std::vector<SXNGN::Tile> tiles)
{
	//Go through the tiles
	for (int i = 0; i < tiles.size(); ++i)
	{
		//If the tile is a wall type tile
		if ((tiles.at(i).getType() == TileType::WALL))
		{
			//If the collision box touches the wall tile
			if (Collision::checkCollision(box, tiles.at(i).getBox()))
			{
				return true;
			}
		}
	}

	//If no wall tiles were touched
	return false;
}
