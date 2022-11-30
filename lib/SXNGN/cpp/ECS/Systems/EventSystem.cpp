
#include<Collision.h>
#include <ECS/Core/Coordinator.hpp>
#include <Database.h>
#include <ECS/Systems/EventSystem.hpp>
#include <ECS/Components/EventComponent.hpp>
#include <fstream>
#include <ECS/Utilities/Entity_Builder_Utils.hpp>

namespace SXNGN::ECS::A {

	void Event_System::Init()
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Event_System Init");
	}

	void Event_System::Update(float dt)
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
				case EventType::SPAWN:
				{
					SDL_LogInfo(1, "Event_System::Update:: Got Spawn Event");
					Handle_Spawn_Event(event_ptr);
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
		float mod = ec->e.mouse_wheel_event.y_ / 2.0;
		SXNGN::Database::modify_scale(mod);
		std::cout << "Scale now: " << SXNGN::Database::get_scale() << std::endl;
		//}
	}

	void Event_System::Handle_Spawn_Event(Event_Component* ec)
	{
		auto gCoordinator = *SXNGN::Database::get_coordinator();
		Entity_Builder_Utils::Create_Tile(gCoordinator, ComponentTypeEnum::MAIN_GAME_STATE, ec->e.spawn_event.x_ / SXNGN::BASE_TILE_WIDTH, ec->e.spawn_event.y_ / SXNGN::BASE_TILE_HEIGHT, "APOCALYPSE_MAP", "BLACK_BORDER", CollisionType::STATIC, "spawned_block");
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