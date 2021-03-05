#pragma once
#include <memory>
#include <SDL.h>
#include <vector>

namespace SXNGN {

	//clicking a button twice in one button = double click
	const Sint32 DOUBLE_CLICK_THRESH_MS = 500;
	//mouse down then mouse up within X = single click
	const Sint32 SINGLE_CLICK_THRESH_MS = 400;

	class UserInputUtils
	{


	public:


		enum class MOUSE_BUTTON : Uint8 {
			LEFT,
			RIGHT,
			CENTER,
			UNKNOWN
		};

		static std::vector<std::vector<SDL_Event>> filter_sdl_events(std::vector<SDL_Event> events);

		static std::pair<size_t, size_t> wasd_to_x_y(std::vector<SDL_Event> events);

		struct MouseCoord {
			bool down = false;
			Sint32 x = 0;
			Sint32 y = 0;
			Uint32 timestamp_down = 0;
			Uint32 timestamp_up = 0;
		};

		struct MouseState {
			MouseCoord left_button;
			MouseCoord right_button;
			MouseCoord middle_button;
		};

		struct Click {
			MOUSE_BUTTON button = MOUSE_BUTTON::UNKNOWN;
			Uint32 time_stamp = 0;
			Sint32 x = 0;
			Sint32 y = 0;
		};
	};


}
