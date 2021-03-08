#include <ECS/Systems/UserInputSystem.hpp>
#include <UI/UserInputUtils.hpp>




	void User_Input_System::Init()
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "User_Input_System Init");
	}

	/// <summary>
	/// User Input System Goals: Translate user input to components that need it
	/// </summary>
	/// <param name="dt"></param>
	void User_Input_System::Update(float dt)
	{

		auto gCoordinator = *SXNGN::Database::get_coordinator();
		std::vector<Entity> entities_to_cleanup;
		//Iterate through entities this system manipulates/converts
		//(Mostly Pre_Renderables) -> Renderables

		auto it_act = m_actable_entities.begin();
		//actable entities for user input system are UserInputCache (vector of sdl events)
		while (it_act != m_actable_entities.end())
		{
			auto const& entity_actable = *it_act;
			it_act++;

			
			auto user_input_ptr = gCoordinator.GetComponentReadOnly(entity_actable, ComponentTypeEnum::INPUT_CACHE);
			User_Input_Cache input_cache = *static_cast<const User_Input_Cache*>(user_input_ptr);

			std::vector<std::vector<SDL_Event>> sorted_events = UserInputUtils::filter_sdl_events(input_cache.events_);
			auto mouse_events = sorted_events.at(0);
			auto keyboard_events = sorted_events.at(1);
			auto mouse_wheel_events = sorted_events.at(2);

			if (mouse_events.empty() == false)
			{
				Update_Mouse_State(mouse_events, dt);
			}

			auto it_inter = m_entities_of_interest.begin();
			//Entities of interest are any entity that need user input
			while (it_inter != m_entities_of_interest.end())
			{
				auto const& entity_interest = *it_inter;
				it_inter++;

				auto tags_ptr = gCoordinator.GetComponentReadOnly(entity_interest, ComponentTypeEnum::INPUT_TAGS);
				User_Input_Tags_Collection input_tags = *static_cast<const User_Input_Tags_Collection*>(tags_ptr);
				if (input_tags.input_tags_.count(User_Input_Tags::WASD_CONTROL))
				{

					if (input_tags.input_tags_.count(User_Input_Tags::PLAYER_CONTROL_MOVEMENT))
					{
						auto check_out_move = gCoordinator.CheckOutComponent(entity_interest, ComponentTypeEnum::MOVEABLE);
						if (check_out_move.first)
						{
							Moveable* moveable_ptr = static_cast<Moveable*>(check_out_move.first);
							Translate_User_Input_To_Movement(moveable_ptr, entity_interest, keyboard_events, dt);
							gCoordinator.CheckInComponent(ComponentTypeEnum::MOVEABLE, entity_interest, std::move(check_out_move.second));
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
				std::pair<Sint32,Sint32> x_y = UserInputUtils::wasd_to_x_y(keyboard_inputs);
				if (x_y.first != 0 || x_y.second!= 0)
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
				printf("Unsupported moveable type : %2d", moveable->moveable_type_);
			}

			}
		}
		
	}



	void User_Input_System::Update_Mouse_State(std::vector<SDL_Event> mouse_events, float dt)
	{
		//mutex controlled singleton
		auto cached_mouse_state = User_Input_State::get_instance();
		SXNGN::UserInputUtils::MouseState current_mouse_state;
		current_mouse_state = cached_mouse_state->mouse_state;
		for (auto e : mouse_events)
		{
			switch (e.type)
			{
			case SDL_MOUSEBUTTONDOWN:
				{
					switch (e.button.button)
					{
						bool left;
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
							current_mouse_state.right_button.down= false;
							current_mouse_state.right_button.x = e.button.x;
							current_mouse_state.right_button.y = e.button.y;
							current_mouse_state.right_button.timestamp_up = e.button.timestamp;
							break;
						}
					}
					//do logic
					Uint32 right_up_minus_down = (current_mouse_state.right_button.timestamp_up - current_mouse_state.right_button.timestamp_down);
					if (right_up_minus_down > 0 && right_up_minus_down < SXNGN::SINGLE_CLICK_THRESH_MS)
					{

						SXNGN::UserInputUtils::Click right_click;
						right_click.button = UserInputUtils::MOUSE_BUTTON::RIGHT;
						right_click.time_stamp = current_mouse_state.right_button.timestamp_up;
						right_click.x = current_mouse_state.right_button.x;
						right_click.y = current_mouse_state.right_button.y;
						
						
						Uint32 click_gap = (right_click.time_stamp - cached_mouse_state->last_right_click.time_stamp);
						cached_mouse_state->last_right_click = right_click;
						if (click_gap < SXNGN::DOUBLE_CLICK_THRESH_MS && click_gap > 0)
						{
							//printf("right double click %zd\n", click_gap);
							cached_mouse_state->last_right_click.time_stamp = 0; //reset after double click to three quick clicks != 2 double clicks
							//todo launc double click event
						}
						else
						{
							//printf("right click %zd\n", click_gap);
							//todo launch single click event
						}
						//clear out the state after recording a click
						current_mouse_state.right_button.timestamp_up = 0;
						current_mouse_state.right_button.timestamp_down = 0;
						
						
					}
					Uint32 left_up_minus_down = (current_mouse_state.left_button.timestamp_up - current_mouse_state.left_button.timestamp_down);
					if (left_up_minus_down > 0 && left_up_minus_down < SXNGN::SINGLE_CLICK_THRESH_MS)
					{

						SXNGN::UserInputUtils::Click left_click;
						left_click.button = UserInputUtils::MOUSE_BUTTON::LEFT;
						left_click.time_stamp = current_mouse_state.left_button.timestamp_up;
						left_click.x = current_mouse_state.left_button.x;
						left_click.y = current_mouse_state.left_button.y;
						
						Uint32 click_gap = (left_click.time_stamp - cached_mouse_state->last_left_click.time_stamp);
						cached_mouse_state->last_left_click = left_click;
						if (click_gap < SXNGN::DOUBLE_CLICK_THRESH_MS && click_gap > 0)
						{
							//printf("left double click %zd\n",click_gap);
							cached_mouse_state->last_left_click.time_stamp = 0; //reset after double click to three quick clicks != 2 double clicks
							//todo launch double click event
						}
						else
						{
							//printf("left click %zd\n", click_gap);
							//todo launch single click event
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

	}
