#pragma once

#include <Sprite/Tile.h>
#include <Collision.h>
#include <vector>
#include <SDL.h>
#include <gameutils.h>

namespace SXNGN {
	//The dot that will move around on the screen
	class Object
	{
	public:

		//Initializes the variables
		Object(std::shared_ptr<Tile> tile, int speed);

		//Takes key presses and adjusts the dot's velocity
		 void handleEvent(SDL_Event& e);

		//Moves the dot and check collision against tiles
		 void move(std::vector<SXNGN::Tile> tiles, SDL_Rect bounds, float time_step = 1);

		//Centers the camera over the dot
		 void setCamera(SDL_Rect& camera, SDL_Rect screen_bounds);

		//Shows the dot on the screen
		 void render(std::shared_ptr<SXNGN::Camera> camera);

		 std::shared_ptr<SXNGN::Tile> get_sprite_ref();

	protected:
		//Collision box of the dot
		SDL_Rect collision_box_;
		std::shared_ptr<SXNGN::Tile> entity_tile_;
		std::string name_ = "Uninit Object";
		int m_vel_x_m_s = 0;//current speed
		int m_vel_y_m_s = 0;
		int m_speed_m_s = 0;//speed added on events

	};
}
