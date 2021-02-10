#pragma once

#include <Tile.h>
#include <Collision.h>
#include <vector>
#include <SDL.h>

namespace SXNGN {
	//The dot that will move around on the screen
	class Dot
	{
	public:
		//The dimensions of the dot
		static const int DOT_WIDTH = 20;
		static const int DOT_HEIGHT = 20;

		//Maximum axis velocity of the dot
		static const int DOT_VEL = 10;

		//Initializes the variables
		Dot();

		//Takes key presses and adjusts the dot's velocity
		void handleEvent(SDL_Event& e);

		//Moves the dot and check collision against tiles
		void move(std::vector<SXNGN::Tile> tiles, SDL_Rect bounds, float time_step = 1);

		//Centers the camera over the dot
		void setCamera(SDL_Rect& camera, SDL_Rect screen_bounds);

		//Shows the dot on the screen
		void render(SDL_Rect& camera, std::shared_ptr<SXNGN::Texture> dot_texture);

	private:
		//Collision box of the dot
		SDL_Rect m_box_;

		//The velocity of the dot
		int mVelX, mVelY;
	};
}
