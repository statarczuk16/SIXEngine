#include <UI/UserInputUtils.hpp>
#include <vector>
#include <ECS/Core/Coordinator.hpp>
#include <ECS/Components/Components.hpp>

namespace SXNGN::ECS {


	std::shared_ptr<UIContainerComponent> UserInputUtils::create_button(kiss_window* parent_window, h_alignment h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, char* name, int row, int column, int width, int height)
	{
		kiss_button* new_kiss_button = new kiss_button();
		kiss_button_new_uc(new_kiss_button, parent_window, name, 40, 0, width, height);
		new_kiss_button->h_align = h_align;
		new_kiss_button->v_align = v_align;
		new_kiss_button->row = row;
		new_kiss_button->column = column;
		new_kiss_button->parent_scale = parent_scale;
		UIContainerComponent new_ui_container(parent_window, layer, UIType::BUTTON);
		new_ui_container.button_ = new_kiss_button;
		//new_ui_container.name_ = name;
		Event_Component back_state_event_ng;
		return std::make_shared<UIContainerComponent>(new_ui_container);
	}

	std::shared_ptr<UIContainerComponent> UserInputUtils::create_select_button(kiss_window* parent_window, h_alignment h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, char* name, int row, int column, int width, int height)
	{
		kiss_selectbutton* new_kiss_toggle = new kiss_selectbutton();
		kiss_selectbutton_new_uc(new_kiss_toggle, parent_window, 40, 0, width, height);
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

	std::shared_ptr<UIContainerComponent> UserInputUtils::create_trading_menu(kiss_window* parent_window, std::string title, std::string detail, UILayer layer, std::map<ItemType, double> shop_inv)
	{
		Coordinator gCoordinator = *SXNGN::Database::get_coordinator();
		sole::uuid player_id = gCoordinator.getUUID(SXNGN::OVERWORLD_PLAYER_UUID);
		Entity player_entity = gCoordinator.GetEntityFromUUID(player_id);
		auto party_data = gCoordinator.GetComponentReadOnly(player_entity, ComponentTypeEnum::PARTY);
		std::map<ItemType, double> player_inv;
		if (party_data)
		{
			const Party* party_ptr = static_cast<const Party*>(party_data);
			player_inv = party_ptr->inventory_;
		}
		std::shared_ptr<SDL_Rect> overworld_viewport = gCoordinator.get_state_manager()->getStateViewPort(ComponentTypeEnum::MAIN_GAME_STATE);
		int w = 300;
		int h = 600;
		int window_layer_int = (int)UILayer::TOP;
		int window_item_layer_int = window_layer_int;
		UILayer window_item_layer = (UILayer)window_item_layer_int;
		int window_x = overworld_viewport->x + (overworld_viewport->w / 2) - (w / 2);
		int window_y = overworld_viewport->y + (overworld_viewport->h / 2) - (h / 2);
		auto trade_window_c = UserInputUtils::create_window_raw(parent_window, window_x, window_y, w, h, layer);

		int player_item_label_x = window_x + 10;
		int player_item_label_w = 50;
		int player_item_label_h = 30;
		
		int row = 2;
		int column = 0;
		for (auto player_item : player_inv)
		{
			std::string item_name = item_type_to_string()[player_item.first];
			std::shared_ptr<UIContainerComponent> title_label_c = UserInputUtils::create_label(trade_window_c->window_, HA_COLUMN, HA_CENTER, VA_ROW, SP_THIRD, window_item_layer, item_name.data(), row, column, player_item_label_w, player_item_label_h);
			trade_window_c->child_components_.push_back(title_label_c);
			row++;
		}

		return trade_window_c;
	}
		

	std::shared_ptr<UIContainerComponent> UserInputUtils::create_message_box(kiss_window* parent_window, std::string title, std::string detail, int w, int h, UILayer layer, std::vector<std::string> option_strings, std::vector<std::function<void()>> option_callbacks, std::vector<bool> option_enables)
	{
		Coordinator gCoordinator = *SXNGN::Database::get_coordinator();
		std::shared_ptr<SDL_Rect> overworld_viewport = gCoordinator.get_state_manager()->getStateViewPort(ComponentTypeEnum::MAIN_GAME_STATE);
		int window_layer_int = (int)layer;
		int window_item_layer_int = window_layer_int;
		UILayer window_item_layer = (UILayer)window_item_layer_int;
		int window_x = overworld_viewport->x + (overworld_viewport->w / 2) - (w / 2);
		int window_y = overworld_viewport->y + (overworld_viewport->h / 2) - (h / 2);
		auto message_window_c = UserInputUtils::create_window_raw(parent_window, window_x, window_y, w, h, layer);
		int column = 0;
		int row = 0;
		char* title_str = title.data();
		std::shared_ptr<UIContainerComponent> title_label_c = UserInputUtils::create_label(message_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, window_item_layer, title_str, row++, -1, 50, 50);
		char* detail_str = detail.data();
		std::shared_ptr<UIContainerComponent> detail_label_c = UserInputUtils::create_label(message_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, window_item_layer, detail_str, row++, -1, 50, 50);
		message_window_c->child_components_.push_back(title_label_c);
		message_window_c->child_components_.push_back(detail_label_c);


		row = 3;
		column = 0;
		scale_to_parent_width button_width = SP_HALF;
		h_alignment h_align = HA_COLUMN;
		switch (option_strings.size())
		{
		case 1: button_width = SP_FILL_WITH_BUFFER; h_align = HA_CENTER; break;
		case 2: button_width = SP_HALF; break;
		case 3: button_width = SP_THIRD; break;
		case 4: button_width = SP_FOURTH; break;
		default: button_width = SP_FOURTH; break;
		}
		for (int i = 0; i < option_strings.size(); i++)
		{
			column = i % 4;
			char* button_str = option_strings[i].data();
			std::shared_ptr<UIContainerComponent> button_c = UserInputUtils::create_button(message_window_c->window_, h_align, VA_ROW, button_width, window_item_layer, button_str, row, column, 50, 50);

			//set button to disabled if does not pass the test passed in
			if (option_enables.size() > i)
			{
				if (!option_enables[i])
				{
					button_c->button_->enabled = false;
				}
			}
			std::function<void(std::shared_ptr<UIContainerComponent> component)> kill_ui = [](std::shared_ptr<UIContainerComponent> uicc)
			{
				uicc->cleanup = true;
			};
			auto kill_button = std::bind(kill_ui, message_window_c);
			button_c->callback_functions_.push_back(kill_button);
			button_c->callback_functions_.push_back(option_callbacks[i]);
			message_window_c->child_components_.push_back(button_c);
		
		}
		


		return message_window_c;

	}


	std::shared_ptr<UIContainerComponent> UserInputUtils::create_window_raw(kiss_window* parent_window, int x, int y, int w, int h, UILayer layer)
	{

		
		kiss_window* new_kiss_window = new kiss_window();
		kiss_window_new(new_kiss_window, parent_window, 1, x, y, w, h);
		new_kiss_window->visible = true;
		UIContainerComponent mmw_container(parent_window, layer, UIType::WINDOW);
		mmw_container.window_ = new_kiss_window;
		return std::make_shared<UIContainerComponent>(mmw_container);
	}

	std::shared_ptr<UIContainerComponent> UserInputUtils::create_label(kiss_window* parent_window, h_alignment h_align, h_alignment txt_h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, char* name, int row, int column, int width, int height)
	{
		kiss_label* new_kiss_label = new kiss_label();
		kiss_label_new_uc(new_kiss_label, parent_window, name, 0, 0, width, height);
		new_kiss_label->h_align = h_align;//label center to parent window window
		new_kiss_label->v_align = v_align;
		new_kiss_label->txt_h_align = txt_h_align;//text center to this label
		new_kiss_label->txt_v_align = v_alignment::VA_CENTER;//text center to this label
		new_kiss_label->row = row;
		new_kiss_label->column = column;
		new_kiss_label->parent_scale = parent_scale;
		UIContainerComponent ngl_c(parent_window, layer, UIType::LABEL);
		ngl_c.label_ = new_kiss_label;
		return std::make_shared<UIContainerComponent>(ngl_c);

	}


	std::shared_ptr<UIContainerComponent> UserInputUtils::create_progressbar_from_callback(kiss_window* parent_window, h_alignment h_align, h_alignment txt_h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, std::function<double()> callback, int row, int column, int width, int height)
	{
		//TODO
		kiss_progressbar* new_kiss_progress = new kiss_progressbar();
		kiss_progressbar_new_uc(new_kiss_progress, parent_window, 0, 0, width, height);
		new_kiss_progress->h_align = h_align;//label center to parent window window
		new_kiss_progress->v_align = v_align;
		new_kiss_progress->txt_h_align = txt_h_align;//text center to this label
		new_kiss_progress->row = row;
		new_kiss_progress->column = column;
		new_kiss_progress->parent_scale = parent_scale;
		UIContainerComponent ngl_c(parent_window, layer, UIType::PROGRESSBAR);
		ngl_c.progressbar_ = new_kiss_progress;
		ngl_c.property_ = "";
		return std::make_shared<UIContainerComponent>(ngl_c);
	}


	std::shared_ptr<UIContainerComponent> UserInputUtils::create_progressbar_from_property(kiss_window* parent_window, h_alignment h_align, h_alignment txt_h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, std::string prop, int row, int column, int width, int height)
	{
		kiss_progressbar* new_kiss_progress = new kiss_progressbar();
		kiss_progressbar_new_uc(new_kiss_progress, parent_window, 0, 0, width, height);
		new_kiss_progress->h_align = h_align;//label center to parent window window
		new_kiss_progress->v_align = v_align;
		new_kiss_progress->txt_h_align = txt_h_align;//text center to this label
		new_kiss_progress->row = row;
		new_kiss_progress->column = column;
		new_kiss_progress->parent_scale = parent_scale;
		UIContainerComponent ngl_c(parent_window, layer, UIType::PROGRESSBAR);
		ngl_c.progressbar_ = new_kiss_progress;
		ngl_c.property_ = prop;
		return std::make_shared<UIContainerComponent>(ngl_c);
	}

	std::shared_ptr<UIContainerComponent> UserInputUtils::create_text_entry(kiss_window* parent_window, h_alignment h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, char* name, int row, int column)
	{
		kiss_entry* new_kiss_entry = new kiss_entry();
		kiss_entry_new(new_kiss_entry, parent_window, 1, "", 0, 0, 400);
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

	std::shared_ptr<UIContainerComponent> UserInputUtils::create_num_entry(kiss_window* parent_window, h_alignment h_align, v_alignment v_align, scale_to_parent_width parent_scale,
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