
#include<Collision.h>
#include <ECS/Core/Coordinator.hpp>
#include <Database.h>
#include <ECS/Systems/EventSystem.hpp>
#include <ECS/Components/EventComponent.hpp>
#include <fstream>
#include <ECS/Utilities/Entity_Builder_Utils.hpp>
#include <ECS/Utilities/ECS_Utils.hpp>

namespace SXNGN::ECS {

	void Event_System::Init()
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Event_System Init");
	}

	void Event_System::Update(double dt)
	{

		auto gCoordinator = *SXNGN::Database::get_coordinator();
		std::vector<Entity> entities_to_cleanup;

		//Iterate through entities this system manipulates/converts
		auto it_act = m_actable_entities.begin();
		//actable entities for event system are entities with event component
		while (it_act != m_actable_entities.end())
		{
			auto const& ec = *it_act;
			it_act++;

			//thread safe checkout of data
			auto check_out_event = gCoordinator.CheckOutComponent(ec, ComponentTypeEnum::EVENT);
			if (check_out_event)
			{

				Event_Component* event_ptr = static_cast<Event_Component*>(check_out_event);
				//act on event
				switch (event_ptr->e.common.type)
				{
				case EventType::STATE_CHANGE:
				{
					SDL_LogInfo(1, "Event_System::Update:: State Change Event");
					auto active_states = gCoordinator.GetActiveGameStates();
					for (auto rs : event_ptr->e.state_change.states_to_remove)
					{
						if (event_ptr->e.state_change.hard_remove)
						{
							gCoordinator.ExtractEntitiesWithMatchingComponent(rs);
						}
						active_states.erase(rs);
					}
					for (auto as : event_ptr->e.state_change.new_states)
					{
						active_states.erase(as);
						active_states.insert(as);
					}
					if (active_states.count(ComponentTypeEnum::OVERWORLD_STATE))
					{
						auto camera = CameraComponent::get_instance();
						auto overworld_player_uuid = gCoordinator.getUUID(SXNGN::OVERWORLD_PLAYER_UUID);
						if (overworld_player_uuid != SXNGN::BAD_UUID)
						{
							auto overworld_player_entity = gCoordinator.GetEntityFromUUID(overworld_player_uuid);
							camera->set_target(overworld_player_entity);
						}
					}
					gCoordinator.GameStateChanged(active_states, true);
					break;
				}
				case EventType::EXIT:
				{
					SDL_LogInfo(1, "Event_System::Update:: Got Exit Event");
					exit(EXIT_SUCCESS);
				}
				case EventType::MOUSE:
				{
					//mouse events are also handled in user_input_system (event "used up" by GUI if GUI is single_click_entities)
					//and handled in collision system (event "used up" by a world object if obj is single_click_entities)
					//mouse events not handled in those two systems are handled here, where check is if the mouse event selects a tile
					Handle_Mouse_Event(event_ptr, ec);
					//SDL_LogInfo(1, "Event_System::Update:: Got Mouse Event");
					break;
				}
				case EventType::MOUSE_WHEEL:
				{
					SDL_LogInfo(1, "Event_System::Update:: Got Mouse Wheel Event");
					Handle_Mouse_Wheel_Event(event_ptr);
					break;
				}
				case EventType::ORDER:
				{
					SDL_LogInfo(1, "Event_System::Update:: Got Order Event");
					Handle_Order_Event(event_ptr);
					break;
				}
				case EventType::LOAD:
				{
					SDL_LogInfo(1, "Event_System::Update:: Got Load Event");
					Handle_Load_Event(event_ptr);
					break;

				}
				case EventType::SAVE:
				{
					SDL_LogInfo(1, "Event_System::Update:: Got Save Event");
					Handle_Save_Event(event_ptr);
					break;

				}
				case EventType::FUNCTION:
				{
					SDL_LogInfo(1, "Event_System::Update:: Got Function Event");
					for (auto func : event_ptr->e.func_event.callbacks)
					{
						func();
					}
					break;

				}
				case EventType::SPAWN:
				{
					SDL_LogInfo(1, "Event_System::Update:: Got Spawn Event");
					Handle_Spawn_Event(event_ptr);
					break;

				}
				case EventType::PARTY:
				{
					SDL_LogInfo(1, "Event_System::Update:: Got Party Event");
					Handle_Party_Event(event_ptr);
					break;

				}
				case EventType::CHOICE:
				{
					SDL_LogInfo(1, "Event_System::Update:: Got Choice Event");
					Handle_Choice_Event(event_ptr);
					break;
				}
				case EventType::SELECTION:
				{
					//todo max number of selected entities
					SDL_LogInfo(1, "Event_System::Update:: Got Selection Event");
					auto user_input_state = User_Input_State::get_instance();
					if (event_ptr->e.select_event.additive)
					{
						for (auto entity : event_ptr->e.select_event.clicked_entities)
						{
							if (gCoordinator.EntityHasComponent(entity, ComponentTypeEnum::SELECTABLE))
							{
								user_input_state->selected_entities.insert(entity);
							}
							
						}
					}
					else if (event_ptr->e.select_event.subtractive)
					{
						for (auto entity : event_ptr->e.select_event.clicked_entities)
						{
							if (gCoordinator.EntityHasComponent(entity, ComponentTypeEnum::SELECTABLE))
							{
								user_input_state->selected_entities.erase(entity);
							}
							
						}
					}
					else if (event_ptr->e.select_event.enqueue)
					{
						//todo job queueing system
					}
					else
					{
						user_input_state->selected_entities.clear();
						for (auto entity : event_ptr->e.select_event.clicked_entities)
						{
							if (gCoordinator.EntityHasComponent(entity, ComponentTypeEnum::SELECTABLE))
							{
								user_input_state->selected_entities.insert(entity);
							}
							
						}
					}

					//list all the entities that are now selected
					SDL_LogInfo(1, "Event_System::Update::Selection --- selected entity list:");
					for (Entity entity : user_input_state->selected_entities)
					{
						SDL_LogDebug(1, "%d",entity);
					}
					
					break;
				}
				default:
				{
					SDL_LogCritical(1, "Event_System::Update:: Unknown event type");
					abort();
				}
				}
				//check data back in
				gCoordinator.CheckInComponent(ComponentTypeEnum::EVENT, ec);

				entities_to_cleanup.push_back(ec);
			}

		}

		for (Entity entity_to_clean : entities_to_cleanup)
		{
			gCoordinator.DestroyEntity(entity_to_clean);
		}
	}

	void Event_System::Handle_Save_Event(Event_Component* ec)
	{
		auto gCoordinator = *SXNGN::Database::get_coordinator();
		auto user_input_state = User_Input_State::get_instance();
		std::vector<std::pair<Entity, Signature>> all_entity_sigs = gCoordinator.Get_All_Entity_Signatures();
		std::ofstream save_stream("game_save.json");
		
		for (auto entity_sig : all_entity_sigs)
		{
			if (!(gCoordinator.EntityHasComponent(entity_sig.first, ComponentTypeEnum::EVENT)))
			{
				sole::uuid uuid = gCoordinator.GetUUIDFromEntity(entity_sig.first);
				if (uuid == BAD_UUID)
				{
					SDL_LogError(1, "Entity has no UUID");
					abort();
				}

				json entity_to_save = gCoordinator.Entity_To_JSON(entity_sig.first, uuid);
				save_stream << std::setw(4) << entity_to_save << std::endl;
				//std::cout << entity_to_save.dump(4) << std::endl;
			}
		}
	}

	void Event_System::Handle_Load_Event(Event_Component* ec)
	{
		auto gCoordinator = *SXNGN::Database::get_coordinator();
		auto user_input_state = User_Input_State::get_instance();
		std::ifstream i("game_save.json"); //FIXME
		json j;
		try 
		{
			while (i >> j)
			{
				std::cout << j.dump(4) << std::endl;
				std::shared_ptr<ExternEntity> extern_entity = gCoordinator.JSON_To_Entity(j);
				gCoordinator.Dump_Spaced_Entity_To_ECS(extern_entity);
			}
		}
		catch (const std::exception& exc)
		{
			std::cerr << exc.what();
			std::cout << j.dump(4) << std::endl;
		}
		Event_Component load_game_state_change;
		load_game_state_change.e.common.type = EventType::STATE_CHANGE;
		load_game_state_change.e.state_change.new_states.push_front(ComponentTypeEnum::MAIN_GAME_STATE);
		load_game_state_change.e.state_change.new_states.push_front(ComponentTypeEnum::OVERWORLD_STATE);
		load_game_state_change.e.state_change.states_to_remove.push_front(ComponentTypeEnum::MAIN_MENU_STATE);
		Entity event_entity = Entity_Builder_Utils::Create_Event(gCoordinator, ComponentTypeEnum::CORE_BG_GAME_STATE, load_game_state_change, "Load Game State Change");
	}

	void Event_System::Handle_Mouse_Event(Event_Component* ec, Entity entity)
	{
		//First, get the grid where the mouse event happened by looking up its location component
		auto gCoordinator = *SXNGN::Database::get_coordinator();
		Event_Component* event_ptr = ec;
		
		Location* location_ptr;
		if (ECS_Component* location_data = gCoordinator.CheckOutComponent(entity, ComponentTypeEnum::LOCATION))
		{
			location_ptr = static_cast<Location*>(location_data);
		}
		else
		{
			SDL_LogError(1, "Mouse Event without Location");
			std::terminate();
			return;
		}
		SDL_LogInfo(1, "Event_System::Update:: Got Mouse Event at %f,%f", location_ptr->m_pos_x_m_, location_ptr->m_pos_y_m_);
		Coordinate mouse_click_grid = location_ptr->GetGridCoordinate();
		gCoordinator.CheckInComponent(entity, ComponentTypeEnum::LOCATION);
		//find all the entities in the single_click_entities grid
		auto entity_map_all = gCoordinator.getSpaceToEntityMap();
		auto entity_map = entity_map_all[SXNGN::DEFAULT_SPACE];
		if (mouse_click_grid.x < entity_map.size() && mouse_click_grid.y < entity_map[mouse_click_grid.x].size())
		{
			std::set<sole::uuid> uuids_clicked = entity_map[mouse_click_grid.x][mouse_click_grid.y];
			std::set<Entity> entities_clicked;
			for (auto id : uuids_clicked)
			{
				Entity entity = gCoordinator.GetEntityFromUUID(id);
				entities_clicked.insert(entity);
			}


			//if it is clickable
			std::vector<Entity> single_click_entities(entities_clicked.size());
			std::vector<Entity> double_click_entities(entities_clicked.size());
			std::vector<Entity> boxed_entities(entities_clicked.size());


			switch (event_ptr->e.mouse_event.type)
			{
				// if it's a click event, get more specific click type
			case MouseEventType::CLICK:
			{

				
				if (event_ptr->e.mouse_event.click.double_click)
				{
					//SDL_LogDebug(1, "Entity %d Double Clicked\n", entities_clicked);
					std::copy(entities_clicked.begin(), entities_clicked.end(), double_click_entities.begin());
				}
				else
				{
					//SDL_LogDebug(1, "Entity %d Clicked\n", other_e);
					std::copy(entities_clicked.begin(), entities_clicked.end(), single_click_entities.begin());
				}
				break;
			}
			//or selection box
			case MouseEventType::BOX:
			{
				//SDL_LogDebug(1, "Entity %d Mouse Boxed\n", other_e);
				std::copy(entities_clicked.begin(), entities_clicked.end(), boxed_entities.begin());
				break;
			}
			default:
			{
				SDL_LogCritical(1, "EventSystem: Unknown Mouse Event");
				abort();
			}
			}//switch mouse event type

			bool additive = false;
			bool subtractive = false;
			bool enqueue = false;
			//priority of modified mouse clicks in this order
			if (event_ptr->e.mouse_event.shift_click)
			{
				enqueue = true;
			}
			else if (event_ptr->e.mouse_event.ctrl_click)
			{
				additive = true;
			}
			else if (event_ptr->e.mouse_event.alt_click)
			{
				subtractive = true;
			}

			//dragged box or left click is selection
			if (event_ptr->e.mouse_event.type == MouseEventType::BOX || event_ptr->e.mouse_event.click.button == MOUSE_BUTTON::LEFT)
			{
				//get where the moveable currently is before this collision occurs

				if (gCoordinator.getSetting("Debug_Spawn_Block").first == 1)
				{
					Entity_Builder_Utils::Create_Spawn_Event(gCoordinator, ComponentTypeEnum::CORE_BG_GAME_STATE, location_ptr->m_pos_x_m_, location_ptr->m_pos_y_m_);
				}
				else
				{

					//create event for user input system - tell it what entities were selected by a mouse event
					Entity_Builder_Utils::Create_Selection_Event(gCoordinator, ComponentTypeEnum::CORE_BG_GAME_STATE, single_click_entities, double_click_entities, boxed_entities, additive, subtractive, enqueue);
				}
			}
			//right click is an order 
			else if (event_ptr->e.mouse_event.click.button == MOUSE_BUTTON::RIGHT)
			{
				//create event for user input system - tell it what the target of the order is 
				//todo different types of orders besides MOVE
				Entity_Builder_Utils::Create_Order_Event(gCoordinator, ComponentTypeEnum::CORE_BG_GAME_STATE, OrderType::MOVE, single_click_entities, double_click_entities, boxed_entities, additive, subtractive, enqueue);
			}
		}
		
	}

	void Event_System::Handle_Mouse_Wheel_Event(Event_Component* ec)
	{
		//auto cached_input_state = User_Input_State::get_instance();
		//if (cached_input_state->selected_entities.size() == 0)
		//{
		double mod = ec->e.mouse_wheel_event.y_ / 2.0;
		SXNGN::Database::modify_scale(mod);
		std::cout << "Scale now: " << SXNGN::Database::get_scale() << std::endl;
		//}
	}

	void Event_System::Handle_Spawn_Event(Event_Component* ec)
	{
		auto gCoordinator = *SXNGN::Database::get_coordinator();
		Entity_Builder_Utils::Create_Tile(gCoordinator, ComponentTypeEnum::MAIN_GAME_STATE, ec->e.spawn_event.x_ / SXNGN::BASE_TILE_WIDTH, ec->e.spawn_event.y_ / SXNGN::BASE_TILE_HEIGHT, "APOCALYPSE_MAP", "BLACK_BORDER", CollisionType::STATIC, "spawned_block");
	}

	void Event_System::Handle_Choice_Event(Event_Component* ec)
	{
		auto ui = UICollectionSingleton::get_instance();
		auto gCoordinator = *SXNGN::Database::get_coordinator();
		
		auto message_box_c = UserInputUtils::create_message_box(nullptr, ec->e.choice_event.title, ec->e.choice_event.detail, 500, 300, UILayer::BOTTOM, ec->e.choice_event.options_text, ec->e.choice_event.options_callbacks, ec->e.choice_event.option_enables);
		if (ec->e.choice_event.pause)
		{
			ECS_Utils::pause_game();
		}
		ui->add_ui_element(ComponentTypeEnum::MAIN_GAME_STATE, message_box_c);
	}

	void Event_System::Handle_Party_Event(Event_Component* ec)
	{
		auto gCoordinator = SXNGN::Database::get_coordinator();
		sole::uuid party_id = ec->e.party_event.party_id;
		Entity party_entity = gCoordinator->GetEntityFromUUID(party_id);
		
		std::cout << "Handling event: " << party_event_type_enum_to_string()[ec->e.party_event.party_event_type] << std::endl;
		switch (ec->e.party_event.party_event_type)
		{
		case PartyEventType::BAD_BOOTS:
		{
			auto ui = UICollectionSingleton::get_instance();
			std::vector<std::string> options_list_text;
			
			std::vector<std::function<void()>> options_list_events;
			std::string detail = "A hole has worn in a pair of boots. Pace will be slowed without footwear.";

			std::function<void()> set_bad_boots = [gCoordinator, party_entity]()
			{
				auto party_component = gCoordinator->CheckOutComponent(party_entity, ComponentTypeEnum::PARTY);
				auto party_ptr = static_cast<Party*>(party_component);
				party_ptr->remove_item(ItemType::FOOTWEAR, 1);
				gCoordinator->CheckInComponent(party_entity, ComponentTypeEnum::PARTY);

			};
			
			options_list_text.push_back("Ok");
			options_list_events.push_back(set_bad_boots);
			std::vector<bool> option_enables;
			auto message_box_c = UserInputUtils::create_message_box(nullptr, "Boots have worn out!", detail, 500, 300, UILayer::BOTTOM, options_list_text, options_list_events, option_enables);
			ECS_Utils::pause_game();
			ui->add_ui_element(ComponentTypeEnum::MAIN_GAME_STATE, message_box_c);
			break;
		}
		case PartyEventType::BAD_LOST:
		{
			auto ui = UICollectionSingleton::get_instance();
			std::vector<std::string> options_list_text;
			
			std::vector<std::function<void()>> options_list_events;
			
			
			std::string detail = "The path is nowhere in sight.";
			
			switch (ec->e.party_event.severity)
			{
			case EventSeverity::MILD: detail = "[MILD] You looked away for a moment, now the road is nowhere in sight! It must be just over the next dune..."; break;
			case EventSeverity::MEDIUM: detail = "[MEDIUM] A terrible gust of wind forced you away from the road. Now it is nowhere in sight."; break;
			case EventSeverity::EXTREME: detail = "[EXTREME] What you thought had been the road for the past few hours turned out not to be. Where could you be now?"; break;
			case EventSeverity::SPICY: detail = "[Spicy!!!] In a thirty stupor, you followed a mirage for an unknowable amount of time. You've come to in an unknown land."; break;
			}
			double severity = (double)ec->e.party_event.severity;
			double lost_counter_inc = severity * LOST_BASE_KM;

			std::function<void()> set_lost = [gCoordinator, party_entity, lost_counter_inc]()
			{
				auto party_component = gCoordinator->CheckOutComponent(party_entity, ComponentTypeEnum::PARTY);
				auto party_ptr = static_cast<Party*>(party_component);
				party_ptr->lost_counter_ = lost_counter_inc;
				party_ptr->lost_counter_max_ = lost_counter_inc;
				ECS_Utils::update_pace();
				gCoordinator->CheckInComponent(party_entity, ComponentTypeEnum::PARTY);

			};
			std::function<void()> use_gps = [gCoordinator, party_entity]()
			{
				auto party_component = gCoordinator->CheckOutComponent(party_entity, ComponentTypeEnum::PARTY);
				auto party_ptr = static_cast<Party*>(party_component);
				std::string result = "";
				int success = party_ptr->use_gps(result);
				gCoordinator->CheckInComponent(party_entity, ComponentTypeEnum::PARTY);
			};
			options_list_text.push_back("Ok");
			options_list_text.push_back("Use GPS");
			options_list_events.push_back(set_lost);
			options_list_events.push_back(use_gps);
			std::vector<bool> option_enables;
			auto message_box_c = UserInputUtils::create_message_box(nullptr, "Lost!", detail, 500, 300, UILayer::BOTTOM, options_list_text, options_list_events, option_enables);

			auto party_component = gCoordinator->CheckOutComponent(party_entity, ComponentTypeEnum::PARTY);
			auto party_ptr = static_cast<Party*>(party_component);
			for (auto option_button : message_box_c->child_components_)
			{
				if (option_button->type_ != UIType::BUTTON)
				{
					continue;
				}
				if (strcmp(option_button->button_->text, "Use GPS") == 0)
				{
					std::string reason_str = "";
					bool button_enabled = party_ptr->can_use_gps(reason_str);
					if (!button_enabled)
					{
						std::string new_button_text = "Use GPS (" + reason_str + ")";
						option_button->button_->enabled = 0;
						new_button_text.copy(option_button->button_->text, new_button_text.size());
						option_button->button_->text[new_button_text.size()] = '\0';
						
					}
				}
			}
			gCoordinator->CheckInComponent(party_entity, ComponentTypeEnum::PARTY);
			ECS_Utils::pause_game();
			ui->add_ui_element(ComponentTypeEnum::MAIN_GAME_STATE, message_box_c);
			break;
		}
		case PartyEventType::BAD_ROBBER:
		{
			auto ui = UICollectionSingleton::get_instance();
			std::vector<std::string> options_list_text;
			
			std::string detail = "Bandits!";
			switch (ec->e.party_event.severity)
			{
			case EventSeverity::MILD: detail = "[MILD] A lone, skinny man in tattered rags brandishes a broken machete."; break;
			case EventSeverity::MEDIUM: detail = "[MEDIUM] A pair of gunslingers draw their pistols."; break;
			case EventSeverity::EXTREME: detail = "[EXTREME] A band of marauders bear their rifles."; break;
			case EventSeverity::SPICY: detail = "[Spicy!!!] An army of vagrants demand your wears or your life."; break;
			}


			std::function<void()> yield_func = [gCoordinator, party_entity]()
			{
				
				
				Event_Component* yield_event_choice = new Event_Component();
				yield_event_choice->e.common.type = EventType::CHOICE;
				yield_event_choice->e.choice_event.title = "Demands";

				auto party_component = gCoordinator->CheckOutComponent(party_entity, ComponentTypeEnum::PARTY);
				auto party_ptr = static_cast<Party*>(party_component);
				bool can_give_food = party_ptr->inventory_[ItemType::FOOD] >= 500;
				bool can_give_batteries = party_ptr->inventory_[ItemType::BATTERY] > 0;
				bool can_give_medkit = party_ptr->inventory_[ItemType::MEDKIT] > 0;
				gCoordinator->CheckInComponent(party_entity, ComponentTypeEnum::PARTY);

				yield_event_choice->e.choice_event.detail = "The bandits demand 500 food, a medkit, or 3 batteries.";

				yield_event_choice->e.choice_event.options_text.push_back("500 food");
				std::function<void()> give_food_func = [gCoordinator, party_entity]()
				{
					auto party_component = gCoordinator->CheckOutComponent(party_entity, ComponentTypeEnum::PARTY);
					auto party_ptr = static_cast<Party*>(party_component);
					party_ptr->remove_item(ItemType::FOOD, 500.0);
					gCoordinator->CheckInComponent(party_entity, ComponentTypeEnum::PARTY);

				};
				yield_event_choice->e.choice_event.options_callbacks.push_back(give_food_func);
				yield_event_choice->e.choice_event.option_enables.push_back(can_give_food);

				yield_event_choice->e.choice_event.options_text.push_back("1 medkit");
				std::function<void()> give_medkit_func = [gCoordinator, party_entity]()
				{
					auto party_component = gCoordinator->CheckOutComponent(party_entity, ComponentTypeEnum::PARTY);
					auto party_ptr = static_cast<Party*>(party_component);
					party_ptr->remove_item(ItemType::MEDKIT, 1);
					gCoordinator->CheckInComponent(party_entity, ComponentTypeEnum::PARTY);

				};
				yield_event_choice->e.choice_event.options_callbacks.push_back(give_medkit_func);
				yield_event_choice->e.choice_event.option_enables.push_back(can_give_medkit);

				yield_event_choice->e.choice_event.options_text.push_back("1 battery");
				std::function<void()> give_battery_func = [gCoordinator, party_entity]()
				{
					auto party_component = gCoordinator->CheckOutComponent(party_entity, ComponentTypeEnum::PARTY);
					auto party_ptr = static_cast<Party*>(party_component);
					party_ptr->remove_item(ItemType::BATTERY, 1);
					gCoordinator->CheckInComponent(party_entity, ComponentTypeEnum::PARTY);

				};
				yield_event_choice->e.choice_event.options_callbacks.push_back(give_battery_func);
				yield_event_choice->e.choice_event.option_enables.push_back(can_give_batteries);

				Entity yield_event_entity = gCoordinator->CreateEntity();
				gCoordinator->AddComponent(yield_event_entity, yield_event_choice);
				gCoordinator->AddComponent(yield_event_entity, Create_Gamestate_Component_from_Enum(ComponentTypeEnum::CORE_BG_GAME_STATE));

			};
			std::function<void()> fight_func = [gCoordinator, party_entity, yield_func]()
			{
				auto party_component = gCoordinator->CheckOutComponent(party_entity, ComponentTypeEnum::PARTY);
				auto party_ptr = static_cast<Party*>(party_component);

				gCoordinator->CheckInComponent(party_entity, ComponentTypeEnum::PARTY);
				
				yield_func();
				
			};
			std::vector<std::function<void()>> options_list_events;
			options_list_text.push_back("Yield");
			options_list_text.push_back("Fight");
			options_list_events.push_back(yield_func);
			options_list_events.push_back(fight_func);
			std::vector<bool> option_enables;
			auto message_box_c = UserInputUtils::create_message_box(nullptr, "Bandits", detail, 500, 300, UILayer::BOTTOM, options_list_text, options_list_events, option_enables);

			auto party_component = gCoordinator->CheckOutComponent(party_entity, ComponentTypeEnum::PARTY);
			auto party_ptr = static_cast<Party*>(party_component);
			for (auto option_button : message_box_c->child_components_)
			{
				if (option_button->type_ != UIType::BUTTON)
				{
					continue;
				}
				if (strcmp(option_button->button_->text, "Fight") == 0)
				{
					std::string reason_str = "";
					int strength = party_ptr->get_fighting_strength(reason_str);
					
					std::string new_button_text = "Fight (Strength: " + std::to_string(strength) + ")";// + "\n" + reason_str + ")";
					new_button_text.copy(option_button->button_->text, new_button_text.size());
					option_button->button_->text[new_button_text.size()] = '\0';
				}
			}
			gCoordinator->CheckInComponent(party_entity, ComponentTypeEnum::PARTY);


			ECS_Utils::pause_game();
			ui->add_ui_element(ComponentTypeEnum::MAIN_GAME_STATE, message_box_c);
			break;
		}
		case PartyEventType::BAD_SICK:
		{
			auto ui = UICollectionSingleton::get_instance();
			std::vector<std::string> options_list_text;

			std::string detail = "Sick.";
			switch (ec->e.party_event.severity)
			{
			case EventSeverity::UNKNOWN: break;
			case EventSeverity::MILD: detail = "[MILD] Feeling a little under the weather."; break;
			case EventSeverity::MEDIUM: detail = "[MEDIUM] Something has definitely upset your stomache."; break;
			case EventSeverity::EXTREME: detail = "[EXTREME] You've come down with a fever. It's hard to even move."; break;
			case EventSeverity::SPICY: detail = "[Spicy!!!] You feel like death."; break;
			
			}
			double severity = (double)ec->e.party_event.severity;
			EventSeverity severity_enum = (EventSeverity)ec->e.party_event.severity;
			double sick_counter_inc = severity * SICK_BASE_MINUTES_GM * 60.0;

			std::function<void()> sick_func = [gCoordinator, party_entity, sick_counter_inc, severity_enum]()
			{
				auto party_component = gCoordinator->CheckOutComponent(party_entity, ComponentTypeEnum::PARTY);
				auto party_ptr = static_cast<Party*>(party_component);

				party_ptr->sick_counter_s_ = sick_counter_inc;
				party_ptr->sick_counter_max_ = sick_counter_inc;
				party_ptr->sick_level_ = severity_enum;
				party_ptr->update_encumbrance();
				gCoordinator->CheckInComponent(party_entity, ComponentTypeEnum::PARTY);

			};
			std::function<void()> use_medkit = [gCoordinator, party_entity]()
			{
				auto party_component = gCoordinator->CheckOutComponent(party_entity, ComponentTypeEnum::PARTY);
				auto party_ptr = static_cast<Party*>(party_component);
				std::string reason = "";
				if (party_ptr->can_use_medit(reason))
				{
					party_ptr->remove_item(ItemType::MEDKIT, 1);
				}
				gCoordinator->CheckInComponent(party_entity, ComponentTypeEnum::PARTY);

			};
			std::vector<std::function<void()>> options_list_events;
			options_list_text.push_back("Ok");
			options_list_text.push_back("Use Medicine");
			options_list_events.push_back(sick_func);
			options_list_events.push_back(use_medkit);
			std::vector<bool> option_enables;
			auto message_box_c = UserInputUtils::create_message_box(nullptr, "Feeling sick...", detail, 500, 300, UILayer::BOTTOM, options_list_text, options_list_events, option_enables);

			auto party_component = gCoordinator->CheckOutComponent(party_entity, ComponentTypeEnum::PARTY);
			auto party_ptr = static_cast<Party*>(party_component);
			for (auto option_button : message_box_c->child_components_)
			{
				if (option_button->type_ != UIType::BUTTON)
				{
					continue;
				}
				if (strcmp(option_button->button_->text, "Use Medicine") == 0)
				{
					std::string reason_str = "";
					bool button_enabled = party_ptr->can_use_medit(reason_str);
					if (!button_enabled)
					{
						std::string new_button_text = "Use Medicine (" + reason_str + ")";
						option_button->button_->enabled = 0;
						new_button_text.copy(option_button->button_->text, new_button_text.size());
						option_button->button_->text[new_button_text.size()] = '\0';
					}
				}
			}
			gCoordinator->CheckInComponent(party_entity, ComponentTypeEnum::PARTY);
			ECS_Utils::pause_game();
			ui->add_ui_element(ComponentTypeEnum::MAIN_GAME_STATE, message_box_c);
			break;
		}
		case PartyEventType::BAD_WEATHER:
		{
			auto ui = UICollectionSingleton::get_instance();
			std::vector<std::string> options_list_text;
			
			std::string detail = "Weather.";
			switch (ec->e.party_event.severity)
			{
			case EventSeverity::MILD: detail = "[MILD] A small sandstorm whips up. It will sting to walk in this."; break;
			case EventSeverity::MEDIUM: detail = "[MEDIUM] A sandstorm blows in. It will last for some time. Walking in this would be dangerous."; break;
			case EventSeverity::EXTREME: detail = "[EXTREME] A massive sandstorm is whipping up. Only a fool would travel into this."; break;
			case EventSeverity::SPICY: detail = "[Spicy!!!] The horizon is engulfed in a sandstorm. Traveling on would be suicide."; break;
			}
			double severity = (double)ec->e.party_event.severity;
			EventSeverity severity_enum = (EventSeverity)ec->e.party_event.severity;
			double weather_counter_inc = severity * WEATHER_BASE_MINUTES_GM * 60.0;

			std::function<void()> weather_func = [gCoordinator, party_entity, weather_counter_inc, severity_enum]()
			{
				auto party_component = gCoordinator->CheckOutComponent(party_entity, ComponentTypeEnum::PARTY);
				auto party_ptr = static_cast<Party*>(party_component);

				party_ptr->weather_counter_s_ = weather_counter_inc;
				party_ptr->weather_counter_max_ = weather_counter_inc;
				party_ptr->weather_level_ = severity_enum;
				party_ptr->update_encumbrance();
				gCoordinator->CheckInComponent(party_entity, ComponentTypeEnum::PARTY);

			};
			std::vector<std::function<void()>> options_list_events;
			options_list_events.push_back(weather_func);
			options_list_text.push_back("Ok");
			std::vector<bool> option_enables;
			auto message_box_c = UserInputUtils::create_message_box(nullptr, "The weather turns foul!", detail, 500, 300, UILayer::BOTTOM, options_list_text, options_list_events, option_enables);
			ECS_Utils::pause_game();
			ui->add_ui_element(ComponentTypeEnum::MAIN_GAME_STATE, message_box_c);
			break;
		}
		case PartyEventType::NONE:
		{
			break;
		}
		default:
		{
			std::cout << "Error: Unimplemented event type: " << ec->e.party_event.party_event_type << std::endl;
			abort();
			break;
		}
		}
		
	}

	void Event_System::Handle_Order_Event(Event_Component* ec)
	{
		auto gCoordinator = *SXNGN::Database::get_coordinator();
		auto user_input_state = User_Input_State::get_instance();
		switch (ec->e.order_event.order_type)
		{
			case OrderType::MOVE:
			{
				Task_Component* new_task = new Task_Component();
				new_task->name_ = "MOVE TO";
				new_task->interruptable_ = true;
				new_task->priority_ = 1;
				new_task->scheduled_ = false;

				new_task->required_units_ = user_input_state->selected_entities;

				WorkChunk walk_chunk;
				if (ec->e.order_event.clicked_entities.empty())
				{
					//didnt click on anything
					return;
				}
				auto move_location = gCoordinator.GetComponentReadOnly(ec->e.order_event.clicked_entities.at(0), ComponentTypeEnum::LOCATION);
				if (!move_location)
				{
					return;
				}
				const Location* collisionable = static_cast<const Location*>(move_location);
				walk_chunk.location_.x = collisionable->m_pos_x_m_;
				walk_chunk.location_.y = collisionable->m_pos_y_m_;
				walk_chunk.skill_level_required_ = 0;
				walk_chunk.skill_required_ = TaskSkill::WALKING;
				walk_chunk.work_required_ = 1;
				
				new_task->tasks_.push_back(walk_chunk);
				auto task_entity = gCoordinator.CreateEntity();
				gCoordinator.AddComponent(task_entity, new_task, false);
				gCoordinator.AddComponent(task_entity, Create_Gamestate_Component_from_Enum(ComponentTypeEnum::MAIN_GAME_STATE));

				break;
			}
			default:
			{
				SDL_LogError(1, "Unknown Order Type");
				break;
			}
		}
		

	}

}