#include <UI/UserInputUtils.hpp>
#include <vector>
#include <ECS/Components/Components.hpp>

namespace SXNGN::ECS::A {


	std::shared_ptr<UIContainerComponent> UserInputUtils::create_button(std::shared_ptr<kiss_window> parent_window, h_alignment h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, char* name, int row, int column, int width, int height)
	{
		kiss_button* new_kiss_button = new kiss_button();
		kiss_button_new_uc(new_kiss_button, parent_window.get(), name, 40, 0, width, height);
		new_kiss_button->h_align = h_align;
		new_kiss_button->v_align = v_align;
		new_kiss_button->row = row;
		new_kiss_button->column = column;
		new_kiss_button->parent_scale = parent_scale;
		UIContainerComponent new_ui_container(parent_window, layer, UIType::BUTTON);
		new_ui_container.button_ = new_kiss_button;
		new_ui_container.name_ = name;
		Event_Component back_state_event_ng;
		return std::make_shared<UIContainerComponent>(new_ui_container);
	}

	std::shared_ptr<UIContainerComponent> UserInputUtils::create_select_button(std::shared_ptr<kiss_window> parent_window, h_alignment h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, char* name, int row, int column, int width, int height)
	{
		kiss_selectbutton* new_kiss_toggle = new kiss_selectbutton();
		kiss_selectbutton_new_uc(new_kiss_toggle, parent_window.get(), 40, 0, width, height);
		new_kiss_toggle->h_align = h_align;
		new_kiss_toggle->v_align = v_align;
		new_kiss_toggle->row = row;
		new_kiss_toggle->column = column;
		new_kiss_toggle->parent_scale = parent_scale;
		new_kiss_toggle->parameter = name;
		UIContainerComponent new_ui_container(parent_window, layer, UIType::SELECT_BUTTON);
		new_ui_container.selectbutton_ = new_kiss_toggle;
		//Event_Component back_state_event_ng;
		return std::make_shared<UIContainerComponent>(new_ui_container);
	}

	std::shared_ptr<UIContainerComponent> UserInputUtils::create_window_raw(std::shared_ptr<kiss_window> parent_window, int x, int y, int w, int h, UILayer layer)
	{
		std::shared_ptr<kiss_window> new_kiss_window = std::make_shared<kiss_window>();
		kiss_window_new(new_kiss_window.get(), parent_window.get(), 1, x, y, w, h);
		new_kiss_window->visible = true;
		UIContainerComponent mmw_container(parent_window, layer, UIType::WINDOW);
		mmw_container.window_ = new_kiss_window;
		return std::make_shared<UIContainerComponent>(mmw_container);
	}

	std::shared_ptr<UIContainerComponent> UserInputUtils::create_label(std::shared_ptr<kiss_window> parent_window, h_alignment h_align, h_alignment txt_h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, char* name, int row, int column, int width, int height)
	{
		kiss_label* new_kiss_label = new kiss_label();
		kiss_label_new_uc(new_kiss_label, parent_window.get(), name, 0, 0, width, height);
		new_kiss_label->h_align = h_align;//label center to parent window window
		new_kiss_label->v_align = v_align;
		new_kiss_label->txt_h_align = txt_h_align;//text center to this label
		new_kiss_label->row = row;
		new_kiss_label->column = column;
		new_kiss_label->parent_scale = parent_scale;
		UIContainerComponent ngl_c(parent_window, layer, UIType::LABEL);
		ngl_c.label_ = new_kiss_label;
		return std::make_shared<UIContainerComponent>(ngl_c);

	}


	std::shared_ptr<UIContainerComponent> UserInputUtils::create_text_entry(std::shared_ptr<kiss_window> parent_window, h_alignment h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, char* name, int row, int column)
	{
		kiss_entry* new_kiss_entry = new kiss_entry();
		kiss_entry_new(new_kiss_entry, parent_window.get(), 1, "", 0, 0, 400);
		new_kiss_entry->h_align = h_align;
		new_kiss_entry->v_align = v_align;
		new_kiss_entry->row = row;
		new_kiss_entry->column = column;
		new_kiss_entry->parent_scale = parent_scale;
		new_kiss_entry->lower_bound = 2;
		new_kiss_entry->upper_bound = 24;
		new_kiss_entry->num_val = 2;
		new_kiss_entry->entry_type = TE_NONE;
		new_kiss_entry->rect.h = 50;

		UIContainerComponent new_container(parent_window, layer, UIType::ENTRY);
		new_container.entry_ = new_kiss_entry;
		return std::make_shared<UIContainerComponent>(new_container);
	}

	std::shared_ptr<UIContainerComponent> UserInputUtils::create_num_entry(std::shared_ptr<kiss_window> parent_window, h_alignment h_align, v_alignment v_align, scale_to_parent_width parent_scale,
		UILayer layer, char* name, double lower_bound, double upper_bound, text_entry_type entry_type, double default_val, int row, int column)
	{
		auto new_container = UserInputUtils::create_text_entry(parent_window, h_align, v_align, parent_scale, layer, name, row, column);
		new_container->entry_->lower_bound = lower_bound;
		new_container->entry_->upper_bound = upper_bound;
		new_container->entry_->num_val = default_val;
		snprintf(new_container->entry_->text, 260, "%g", default_val);
		new_container->entry_->entry_type = entry_type;
		return new_container;
		
	}

	/// <summary>
	/// Sort a vector of SDL events into only the ones the game cares about
	/// </summary>
	/// <param name="events"></param>
	/// <returns></returns>
	std::vector<std::vector<SDL_Event>> UserInputUtils::filter_sdl_events(std::vector<SDL_Event> events)
	{
		std::vector<SDL_Event> mouse_events;
		std::vector<SDL_Event> keyboard_events;
		std::vector<SDL_Event> mouse_wheel_events;

		for (auto e : events)
		{
			if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
			{
				mouse_events.push_back(e);
			}
			else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
			{
				keyboard_events.push_back(e);
				//mouse events interested in these modify types
				switch (e.key.keysym.sym)
				{
				case SDLK_LSHIFT: mouse_events.push_back(e); break;
				case SDLK_LALT: mouse_events.push_back(e); break;
				case SDLK_LCTRL: mouse_events.push_back(e); break;
				}
			}
			else if(e.type == SDL_MOUSEWHEEL)
			{
				mouse_wheel_events.push_back(e);
			}
		}

		std::vector<std::vector<SDL_Event>> ret = { mouse_events, keyboard_events, mouse_wheel_events };
		return ret;
	}

	std::pair<Sint32, Sint32> UserInputUtils::wasd_to_x_y(std::vector<SDL_Event> events)
	{
		Sint32 x = 0;
		Sint32 y = 0;
		Sint32 add_amount = 1;
		for (auto event : events)
		{
			auto e = event;
			if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
			{
				//Adjust the velocity
				switch (e.key.keysym.sym)
				{
				case SDLK_UP: y -= add_amount; break;
				case SDLK_DOWN: y += add_amount; break;
				case SDLK_LEFT: x -= add_amount; break;
				case SDLK_RIGHT: x += add_amount; break;
				case SDLK_LSHIFT: add_amount = 2; break; //go twice as fast if holding shift
				}
			}

			if (e.type == SDL_KEYUP && e.key.repeat == 0)
			{
				//Adjust the velocity
				switch (e.key.keysym.sym)
				{
				case SDLK_UP: y += add_amount; break;
				case SDLK_DOWN: y -= add_amount; break;
				case SDLK_LEFT: x += add_amount; break;
				case SDLK_RIGHT: x -= add_amount; break;
				case SDLK_LSHIFT: add_amount = 1; break;
				}
			}
		}
		return std::make_pair(x, y);

	}

	/**
	 UserInputUtils::mouse_movement_to_mouse_state(std::vector<SDL_Event> events)
	{
		size_t x = 0.0;
		size_t y = 0.0;
		size_t add_amount = 1;
		for (auto event : events)
		{
			auto e = event;
			if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
			{
				//Adjust the velocity
				switch (e.key.keysym.sym)
				{
				case SDLK_UP: y -= add_amount; break;
				case SDLK_DOWN: y += add_amount; break;
				case SDLK_LEFT: x -= add_amount; break;
				case SDLK_RIGHT: x += add_amount; break;
				case SDLK_LSHIFT: add_amount = 2; break; //go twice as fast if holding shift
				}
			}

			if (e.type == SDL_KEYUP && e.key.repeat == 0)
			{
				//Adjust the velocity
				switch (e.key.keysym.sym)
				{
				case SDLK_UP: y += add_amount; break;
				case SDLK_DOWN: y -= add_amount; break;
				case SDLK_LEFT: x += add_amount; break;
				case SDLK_RIGHT: x -= add_amount; break;
				case SDLK_LSHIFT: add_amount = 1; break;
				}
			}
		}
		return std::make_pair(x, y);

	} **/
}