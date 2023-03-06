#pragma once
#include <memory>
#include <SDL.h>
#include <vector>
#include <kiss_sdl.h>
#include <ECS/Core/Types.hpp>




namespace SXNGN::ECS {


	//clicking a button twice in one button = double cl ick
	const Sint32 DOUBLE_CLICK_THRESH_MS = 300;
	//mouse down then mouse up within X = single click
	const Sint32 SINGLE_CLICK_THRESH_MS = 400;

	struct UIContainerComponent;

	class UserInputUtils
	{


	public:

		static std::vector<std::vector<SDL_Event>> filter_sdl_events(std::vector<SDL_Event> events);

		static std::pair<Sint32, Sint32> wasd_to_x_y(std::vector<SDL_Event> events);

		static std::shared_ptr<UIContainerComponent> create_button(kiss_window* parent_window, h_alignment h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, char* name, int row = -1, int column = -1, int width = -1, int height = -1);

		static std::shared_ptr<UIContainerComponent> create_select_button(kiss_window* parent_window, h_alignment h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, char* name, int row, int column, int width, int height);

		static std::shared_ptr<UIContainerComponent> create_window_raw(kiss_window* parent_window, int x, int y, int w, int h, UILayer layer);

		static std::shared_ptr<UIContainerComponent> create_label(kiss_window* parent_window, h_alignment h_align, h_alignment txt_h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, char* name, int row = -1, int column = -1, int width = -1, int height = -1);

		static std::shared_ptr<UIContainerComponent> create_progressbar_from_property(kiss_window* parent_window, h_alignment h_align, h_alignment txt_h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, std::string prop, int row = -1, int column = -1, int width = -1, int height = -1);

		static std::shared_ptr<UIContainerComponent> create_progressbar_from_callback(kiss_window* parent_window, h_alignment h_align, h_alignment txt_h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, std::function<double()> callback = nullptr, int row = -1, int column = -1, int width = -1, int height = -1);


		static std::shared_ptr<UIContainerComponent> create_text_entry(kiss_window* parent_window, h_alignment h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, char* name, int row = -1, int column = -1);

		static std::shared_ptr<UIContainerComponent> create_num_entry(kiss_window* parent_window, h_alignment h_align, v_alignment v_align, scale_to_parent_width parent_scale,
			UILayer layer, char* name, double lower_bound, double upper_bound, text_entry_type entry_type, double default_val = 0, int row = -1, int column = -1);

		static std::shared_ptr<UIContainerComponent> create_message_box(kiss_window* parent_window, std::string title, int w, int h, UILayer layer, std::vector<std::string> option_strings, std::vector<Event_Component*> option_events);


		
	};


}
