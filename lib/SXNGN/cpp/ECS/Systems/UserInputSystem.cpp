#include <ECS/Systems/UserInputSystem.hpp>
#include <UI/UserInputUtils.hpp>
#include <ECS/Core/Coordinator.hpp>
#include <ECS/Utilities/Entity_Builder_Utils.hpp>

#include <stack>
#include <iostream>
#include <Physics.h>


namespace SXNGN::ECS::A {

	void User_Input_System::Init()
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "User_Input_System Init");
	}

	/// <summary>
	/// User Input A Goals: Translate user input to components that need it
	/// </summary>
	/// <param name="dt"></param>
	void User_Input_System::Update(float dt)
	{
		auto gCoordinator = *SXNGN::Database::get_coordinator();
		std::vector<Entity> entities_to_cleanup;

		//SDL_LogDebug(1, "User_Input_Begin ");
		auto it_act = m_actable_entities.begin();
		//actable entities for user input system are UserInputCache (vector of sdl events)
		auto total = m_actable_entities.size();
		while (it_act != m_actable_entities.end())
		{
			auto const& entity_actable = *it_act;
			it_act++;
			auto user_input_ptr = gCoordinator.GetComponentReadOnly(entity_actable, ComponentTypeEnum::INPUT_CACHE);
			User_Input_Cache input_cache = *static_cast<const User_Input_Cache*>(user_input_ptr);

			if (input_cache.events_.empty() == false)
			{
				//pass by ref
				Handle_GUI_Input(input_cache.events_);
			}

			std::vector<std::vector<SDL_Event>> sorted_events = UserInputUtils::filter_sdl_events(input_cache.events_);
			std::vector<SDL_Event> mouse_events = sorted_events.at(0);
			std::vector<SDL_Event> keyboard_events = sorted_events.at(1);
			std::vector<SDL_Event> mouse_wheel_events = sorted_events.at(2);

			if (mouse_events.empty() == false || mouse_wheel_events.empty() == false)
			{
				//Pass mouse events by reference - this function can remove events from the vector 
				Update_Mouse_State(mouse_events, mouse_wheel_events, dt);
			}

			auto it_inter = m_entities_of_interest.begin();
			//Entities of interest are any entity that need user input
			
			while (it_inter != m_entities_of_interest.end())
			{
				auto const& entity_interest = *it_inter;
				it_inter++;

				if (keyboard_events.empty() == false)
				{
					auto tags_ptr = gCoordinator.GetComponentReadOnly(entity_interest, ComponentTypeEnum::INPUT_TAGS);
					if (tags_ptr)
					{
						User_Input_Tags_Collection input_tags = *static_cast<const User_Input_Tags_Collection*>(tags_ptr);
						if (input_tags.input_tags_.count(User_Input_Tags::WASD_CONTROL))
						{
							if (input_tags.input_tags_.count(User_Input_Tags::PLAYER_CONTROL_MOVEMENT))
							{
								auto check_out_move = gCoordinator.CheckOutComponent(entity_interest, ComponentTypeEnum::MOVEABLE);
								if (check_out_move)
								{
									Moveable* moveable_ptr = static_cast<Moveable*>(check_out_move);
									Translate_User_Input_To_Movement(moveable_ptr, entity_interest, keyboard_events, dt);
									gCoordinator.CheckInComponent(ComponentTypeEnum::MOVEABLE, entity_interest);
								}
							}
						}
					}
				}
				
			}
			entities_to_cleanup.push_back(entity_actable);
		}
		
		for (Entity entity_to_clean : entities_to_cleanup)
		{
			gCoordinator.DestroyEntity(entity_to_clean);
		}
		
	}

	

	void User_Input_System::Translate_User_Input_To_Movement(Moveable* moveable, Entity entity, std::vector<SDL_Event> keyboard_inputs, float dt)
	{

		if (moveable)
		{
			switch (moveable->moveable_type_)
			{
			case MoveableType::VELOCITY:
			{
				std::pair<Sint32, Sint32> x_y = UserInputUtils::wasd_to_x_y(keyboard_inputs);
				if (x_y.first != 0 || x_y.second != 0)
				{
					//printf("UserInputSystem: Moving Entity %2d: Velocity: (%d,%d)\n",entity, x_y.first, x_y.second);
				}
				moveable->m_vel_x_m_s += (x_y.first * moveable->m_speed_m_s);
				moveable->m_vel_y_m_s += (x_y.second * moveable->m_speed_m_s);
			}
			break;
			case MoveableType::FORCE:
			{
				printf("Physics movement not yet supported");
			}
			break;
			default:
			{
				printf("Unsupported moveable type_ : %2d", moveable->moveable_type_);
			}

			}
		}

	}

	void User_Input_System::Handle_GUI_Input(std::vector<SDL_Event>& events)
	{
		auto gCoordinator = *SXNGN::Database::get_coordinator();
		auto ui = UICollectionSingleton::get_instance();

		std::forward_list<ComponentTypeEnum> active_game_states = gCoordinator.GetActiveGameStates();

		for (auto state : active_game_states)
		{
			auto game_state_ui = ui->state_to_ui_map_.find(state);
			if (game_state_ui != ui->state_to_ui_map_.end())
			{
				GUI_Handle_Events(events, game_state_ui->second);
			}
			
		}
	}


	void User_Input_System::GUI_Handle_Events(std::vector<SDL_Event> &events, std::map<UILayer, std::vector<std::shared_ptr<UIContainerComponent>>> layer_to_ui_elements)
	{
		int draw_ui = 1;
		auto coordinator = Database::get_coordinator();
		SDL_Renderer* gRenderer = coordinator->Get_Renderer();

		// Iterate through all GUI components on this layer

		//Iterate backward so UI Components on higher layers 'absorb' events first
		for (std::map<UILayer, std::vector<std::shared_ptr<UIContainerComponent>>>::reverse_iterator layer_it = layer_to_ui_elements.rbegin(); layer_it != layer_to_ui_elements.rend(); layer_it++)
		{
			
			int event_idx = 0;
			//Events are handled by layer and any event can be handled by ALL elements on a layer, but not by elements across two layers
			//Logic: For each layer, go through each event and see which elements it hits. If it hits any elements on layer X, add the index of the event to the events_handled stack.
			//After the events have been iterated for layer X, go through the stack and remove any events that were handled by the layer.
			//Use a std::stack for the last in, first out property (when deleting multiple indices from a vector, you need to go from highest index to lowest index).
			//Use case: A text box that can be "active" or "inactive" needs to know about a click event even if the click event did not hit the textbox. (activate if clicked on, deactivated on a click anywhere else on its layer).
			std::stack<int> events_handled_this_layer;
			std::vector<SDL_Event>::iterator event_it = events.begin();
			while (event_it != events.end())
			{
				SDL_Event* e = &*event_it;
				bool event_handled = false;
				
				for (auto component_in_layer : layer_it->second)
				{
					bool event_handled_temp = GUI_Handle_Event(coordinator, e, component_in_layer);
					event_handled = event_handled || event_handled_temp;
				}
				if (event_handled)
				{
					events_handled_this_layer.push(event_idx);
				}
				event_it++;
				event_idx++;
			} //handle event loop

			while (events_handled_this_layer.size() != 0)
			{
				int to_pop = events_handled_this_layer.top();
				//printf("popping from events handled %d\n", to_pop);
				events.erase(events.begin() + to_pop);
				events_handled_this_layer.pop();
			}
		} //handle layer loop
	}

	bool User_Input_System::GUI_Handle_Event(std::shared_ptr<Coordinator> coordinator, SDL_Event* e, std::shared_ptr<UIContainerComponent> component_in_layer)
	{
		int draw_ui = 1;
		bool event_handled = false;
		if (component_in_layer == nullptr)
		{
			return false;
		}
		switch (component_in_layer->type_)
		{
			case UIType::ENTRY:
			{
				int entry_status = kiss_entry_event(component_in_layer->entry_, e, &draw_ui);
				if (entry_status > 0)
				{
					event_handled = true;
					if (entry_status == 2)
					{
						//SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "GUI_HandleEvents: Button: %s activated", component_in_layer->name_.c_str());
						for (Event_Component triggered_event : component_in_layer->triggered_events)
						{
							Entity event_entity = Entity_Builder_Utils::Create_Event(*coordinator, ComponentTypeEnum::CORE_BG_GAME_STATE, triggered_event, "Entry Event");
						}
						for (auto func : component_in_layer->callback_functions_)
						{
							func();
						}
					}
				}
				break;
			}
			case UIType::WINDOW:
			{
				if (kiss_window_event(component_in_layer->window_.get(), e, &draw_ui))
				{
					event_handled = true;
				}
				// WARNING: we use a deque because of its iterator invalidation properties
				//(Elements can be added/removed mid loop)
				std::deque<std::shared_ptr<UIContainerComponent>> child_components(component_in_layer->child_components_.begin(), component_in_layer->child_components_.end());
				//process events for all children components of window
				for (std::deque<std::shared_ptr<UIContainerComponent>>::const_iterator it = child_components.begin(); it != child_components.end(); ++it)
				{
					bool event_handled_child = GUI_Handle_Event(coordinator, e, *it);
					event_handled = event_handled_child || event_handled;
				}
				break;
			}
			case UIType::BUTTON:
			{
				if (kiss_button_event(component_in_layer->button_, e, &draw_ui))
				{
					//SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "GUI_HandleEvents: Button: %s activated", component_in_layer->name_.c_str());
					for (Event_Component triggered_event : component_in_layer->triggered_events)
					{
						Entity event_entity = Entity_Builder_Utils::Create_Event(*coordinator, ComponentTypeEnum::CORE_BG_GAME_STATE, triggered_event, "Button Event");
					}
					for (auto func : component_in_layer->callback_functions_)
					{
						func();
					}

					event_handled = true;
				}
				break;
			}
			case UIType::COMBOBOX:
			{
				break;
			}
			case UIType::HSCROLLBAR:
			{
				break;
			}
			case UIType::PROGRESSBAR:
			{
				break;
			}
			case UIType::SELECT_BUTTON:
			{
				break;
			}
			case UIType::TEXTBOX:
			{
				break;
			}
			case UIType::VSCROLLBAR:
			{
				break;
			}
			case UIType::LABEL:
			{
				break;
			}
			case UIType::UNKNOWN:
			{

			}
			default:
			{
				//printf("User_Input_System::GUI_Handle_Events Unknown UI Component Type");
				abort();
			}
		}
		return event_handled;
	}



	void User_Input_System::Update_Mouse_State(std::vector<SDL_Event> mouse_events, std::vector<SDL_Event> mouse_wheel_events, float dt)
	{
		auto gCoordinator = Database::get_coordinator();
		//mutex controlled singleton
		auto cached_mouse_state = User_Input_State::get_instance();
		MouseState current_mouse_state;
		current_mouse_state = cached_mouse_state->mouse_state;

		//first check for modifying key strokes for shift+click etc
		for (auto e : mouse_events)
		{
			if (e.type == SDL_KEYDOWN)
			{
				//mouse events interested in these modify types
				switch (e.key.keysym.sym)
				{
				case SDLK_LSHIFT: cached_mouse_state->shift_down = true; break;
				case SDLK_LALT: cached_mouse_state->alt_down = true; break;
				case SDLK_LCTRL:cached_mouse_state->ctrl_down = true; break;
				}
			}
			else if (e.type == SDL_KEYUP)
			{
				//mouse events interested in these modify types
				switch (e.key.keysym.sym)
				{
				case SDLK_LSHIFT: cached_mouse_state->shift_down = false; break;
				case SDLK_LALT: cached_mouse_state->alt_down = false; break;
				case SDLK_LCTRL:cached_mouse_state->ctrl_down = false; break;
				}
			}
		}

		bool shift_down = cached_mouse_state->shift_down;
		bool alt_down = cached_mouse_state->alt_down;
		bool ctrl_down = cached_mouse_state->ctrl_down;

		for (auto e : mouse_events)
		{
			switch (e.type)
			{
			case SDL_KEYDOWN: break;
			case SDL_KEYUP: break;
			case SDL_MOUSEBUTTONDOWN:
			{
				switch (e.button.button)
				{
					
				case SDL_BUTTON_LEFT:
				{
					current_mouse_state.left_button.down = true;
					current_mouse_state.left_button.x = e.button.x;
					current_mouse_state.left_button.y = e.button.y;
					current_mouse_state.left_button.timestamp_down = e.button.timestamp;
					break;
				}
				case SDL_BUTTON_RIGHT:
				{
					current_mouse_state.right_button.down = true;
					current_mouse_state.right_button.x = e.button.x;
					current_mouse_state.right_button.y = e.button.y;
					current_mouse_state.right_button.timestamp_down = e.button.timestamp;
					break;
				}
				}

				cached_mouse_state->mouse_state = current_mouse_state;
				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				switch (e.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
					current_mouse_state.left_button.down = false;
					current_mouse_state.left_button.x = e.button.x;
					current_mouse_state.left_button.y = e.button.y;
					current_mouse_state.left_button.timestamp_up = e.button.timestamp;
					break;
				}
				case SDL_BUTTON_RIGHT:
				{
					current_mouse_state.right_button.down = false;
					current_mouse_state.right_button.x = e.button.x;
					current_mouse_state.right_button.y = e.button.y;
					current_mouse_state.right_button.timestamp_up = e.button.timestamp;
					break;
				}
				}
				//do logic
				Uint32 right_up_minus_down = (current_mouse_state.right_button.timestamp_up - current_mouse_state.right_button.timestamp_down);
				if (right_up_minus_down > 0 && right_up_minus_down <SINGLE_CLICK_THRESH_MS)
				{

					Click right_click;
					right_click.button = MOUSE_BUTTON::RIGHT;
					right_click.time_stamp = current_mouse_state.right_button.timestamp_up;
					right_click.x = current_mouse_state.right_button.x;
					right_click.y = current_mouse_state.right_button.y;


					Uint32 click_gap = (right_click.time_stamp - cached_mouse_state->last_right_click.time_stamp);
					cached_mouse_state->last_right_click = right_click;
					if (click_gap < DOUBLE_CLICK_THRESH_MS && click_gap > 0)
					{
						//printf("right double click %zd\n", click_gap);
						cached_mouse_state->last_right_click.time_stamp = 0; //reset after double click to three quick clicks != 2 double clicks
						//todo launc double click event
						right_click.double_click = true;
						Entity_Builder_Utils::Create_Mouse_Event(*gCoordinator, ComponentTypeEnum::CORE_BG_GAME_STATE, right_click, shift_down, alt_down, ctrl_down);
					}
					else
					{
						//printf("right click %zd\n", click_gap);
						//todo launch single click event
						right_click.double_click = false;
						Entity_Builder_Utils::Create_Mouse_Event(*gCoordinator, ComponentTypeEnum::CORE_BG_GAME_STATE, right_click, shift_down, alt_down, ctrl_down);
					}
					//clear out the state after recording a click
					current_mouse_state.right_button.timestamp_up = 0;
					current_mouse_state.right_button.timestamp_down = 0;


				}
				Uint32 left_up_minus_down = (current_mouse_state.left_button.timestamp_up - current_mouse_state.left_button.timestamp_down);
				if (left_up_minus_down > 0 && left_up_minus_down < SINGLE_CLICK_THRESH_MS)
				{

					Click left_click;
					left_click.button = MOUSE_BUTTON::LEFT;
					left_click.time_stamp = current_mouse_state.left_button.timestamp_up;
					left_click.x = current_mouse_state.left_button.x;
					left_click.y = current_mouse_state.left_button.y;

					Uint32 click_gap = (left_click.time_stamp - cached_mouse_state->last_left_click.time_stamp);
					cached_mouse_state->last_left_click = left_click;
					if (click_gap < DOUBLE_CLICK_THRESH_MS && click_gap > 0)
					{
						//printf("left double click %zd\n",click_gap);
						cached_mouse_state->last_left_click.time_stamp = 0; //reset after double click to three quick clicks != 2 double clicks
						//todo launch double click event
						left_click.double_click = true;
						Entity_Builder_Utils::Create_Mouse_Event(*gCoordinator, ComponentTypeEnum::CORE_BG_GAME_STATE, left_click, shift_down, alt_down, ctrl_down);

					}
					else
					{
						//printf("left click %zd\n", click_gap);
						// 
						//todo launch single click event
						left_click.double_click = false;
						Entity_Builder_Utils::Create_Mouse_Event(*gCoordinator, ComponentTypeEnum::CORE_BG_GAME_STATE, left_click, shift_down, alt_down, ctrl_down);
					}

				}
				//clear out the state after recording a click
				current_mouse_state.left_button.timestamp_up = 0;
				current_mouse_state.left_button.timestamp_down = 0;
				cached_mouse_state->mouse_state = current_mouse_state;
				break;

			}
			case SDL_MOUSEMOTION:
			{

				//do logic
				cached_mouse_state->mouse_state = current_mouse_state;
				break;
			}

			default:
			{
				printf("Error: Update_Mouse_State got unrecognized SDL Event. Check UserInputUtils filter events and UserInputSystem Update_Mouse_State\n");
				abort();
			}

			}
		}
		for (auto e : mouse_wheel_events)
		{
			switch (e.type)
			{
			case SDL_MOUSEWHEEL:
			{
				int x = e.wheel.x;
				int y = e.wheel.y;
				if (e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
				{
					x *= -1;
					y *= -1;
				}
				
				Entity_Builder_Utils::Create_Mouse_Wheel_Event(*gCoordinator, ComponentTypeEnum::CORE_BG_GAME_STATE, x, y);
			}
			}
		}

	}
}
