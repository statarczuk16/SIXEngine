#pragma once
#include <memory>
#include <SDL.h>

namespace SXNGN {
	//The dot that will move around on the screen
	class Tile;

	class Camera
	{
	public:
		//Initializes the variables
		Camera(SDL_Rect lens, SDL_Rect position, SDL_Rect screen_bounds);

		void lock_on(std::shared_ptr<SXNGN::Tile> target);

		//Moves the dot and check collision against tiles
		void move(float time_step = 1);
		bool object_in_view(SDL_Rect obj_bounds);
		SDL_Rect get_lens_rect_scaled();
		SDL_Rect get_lens_rect_actual();

	protected:
		SDL_Rect lens_;
		SDL_Rect position_actual_; //relation to the level
		SDL_Rect position_scaled_;//scaled (old position * scaling factor)
		SDL_Rect screen_bounds_;
		std::shared_ptr<SXNGN::Tile> target_;
	};
}
