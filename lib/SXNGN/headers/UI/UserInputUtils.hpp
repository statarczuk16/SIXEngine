#pragma once
#include <memory>
#include <SDL.h>
#include <vector>
#include <ECS/Components/UI.hpp>


namespace SXNGN::ECS::A {

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

		static std::pair<Sint32, Sint32> wasd_to_x_y(std::vector<SDL_Event> events);

		static std::shared_ptr<UIContainerComponent> create_button(std::shared_ptr<kiss_window> parent_window, h_alignment h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, char* name, int row = -1, int column = -1);

		static std::shared_ptr<UIContainerComponent> create_window_raw(std::shared_ptr<kiss_window> parent_window, int x, int y, int w, int h, UILayer layer);

		static std::shared_ptr<UIContainerComponent> create_label(std::shared_ptr<kiss_window> parent_window, h_alignment h_align, h_alignment txt_h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, char* name, int row = -1, int column = -1);

		static std::shared_ptr<UIContainerComponent> create_text_entry(std::shared_ptr<kiss_window> parent_window, h_alignment h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, char* name, int row = -1, int column = -1);

		static std::shared_ptr<UIContainerComponent> create_num_entry(std::shared_ptr<kiss_window> parent_window, h_alignment h_align, v_alignment v_align, scale_to_parent_width parent_scale,
			UILayer layer, char* name, double lower_bound, double upper_bound, text_entry_type entry_type, double default_val = 0, int row = -1, int column = -1);

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
			bool double_click = false;
		};
	};


}
