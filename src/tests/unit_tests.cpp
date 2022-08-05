#include <nlohmann/json.hpp>
#include <iostream>
#include <sole.hpp>
#include <SDL.h>
#include <fstream>
#include <Collision.h>


std::vector<std::function<void()>> tests;

int test_1()
{
	int radius = 5;
	SDL_Rect rect;
	rect.x = 10;
	rect.y = 0;
	rect.w = 5;
	rect.h = 5;

	SDL_Rect circle;
	circle.x = 0;
	circle.y = 0;
	circle.w = radius;
	circle.y = radius;

	for (int x = 0; x < 40; x++)
	{
		circle.x = x;
		bool collide = SXNGN::CollisionChecks::checkCollisionRectCircleBuffer(rect, circle, 0);

		std::cout << "Radius: " << radius << std::endl;
		for (int i = 0; i < 40; i++)
		{
			if (x == i)
			{
				std::cout << "O";
			}
			else if (x - radius == i)
			{
				std::cout << "(";
			}
			else if (x + radius == i)
			{
				std::cout << ")";
			}
			else
			{
				std::cout << "_";
			}
		}
		std::cout << std::endl;
		for (int i = 0; i < 40; i++)
		{
			if (i == rect.x)
			{
				std::cout << "[";
			}
			else if (i == rect.x + rect.w)
			{
				std::cout << "]";
			}
			else
			{
				std::cout << "_";
			}
		}
		std::cout << std::endl;
		std::cout << collide << std::endl;

		if (x + radius > rect.x && x - radius < rect.x + rect.w )
		{
			assert(collide == true);
		}
		else
		{
			assert(collide == false);
		}
	}

	


	return 0;

}



int main(int argc, char* args[])
{
	tests.push_back(test_1);

	for (auto test : tests)
	{
		test();
	}
	
	

	return 0;
}