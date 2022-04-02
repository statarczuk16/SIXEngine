
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
			auto const& entity_actable = *it_act;
			it_act++;

			//thread safe checkout of data
			auto check_out_event = gCoordinator.CheckOutComponent(entity_actable, ComponentTypeEnum::EVENT);
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
						active_states.remove(rs);
					}
					for (auto as : event_ptr->e.state_change.new_states)
					{
						active_states.remove(as);
						active_states.push_front(as);
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
					SDL_LogInfo(1, "Event_System::Update:: Got Mouse Event");
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
				case EventType::SELECTION:
				{
					//todo max number of selected entities
					SDL_LogInfo(1, "Event_System::Update:: Got Selection Event");
					auto user_input_state = User_Input_State::get_instance();
					if (event_ptr->e.select_event.additive)
					{
						for (auto entity : event_ptr->e.select_event.clicked_entities)
						{
							user_input_state->selected_entities.insert(entity);
						}
					}
					else if (event_ptr->e.select_event.subtractive)
					{
						for (auto entity : event_ptr->e.select_event.clicked_entities)
						{
							user_input_state->selected_entities.erase(entity);
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
							user_input_state->selected_entities.insert(entity);
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
				gCoordinator.CheckInComponent(ComponentTypeEnum::EVENT, entity_actable);

				entities_to_cleanup.push_back(entity_actable);
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
		std::ofstream save_stream("world.json");
		
		for (auto entity_sig : all_entity_sigs)
		{
			if (gCoordinator.EntityHasComponent(entity_sig.first, ComponentTypeEnum::TILE))
			{
				sole::uuid uuid = gCoordinator.GetUUIDFromEntity(entity_sig.first);
				if (uuid == BAD_UUID)
				{
					SDL_LogError(1, "Entity has no UUID");
					abort();
				}

				json world_tile_to_save = gCoordinator.Entity_To_JSON(entity_sig.first, uuid);
				save_stream << std::setw(4) << world_tile_to_save << std::endl;
				//std::cout << world_tile_to_save.dump(4) << std::endl;
			}
		}
	}

	void Event_System::Handle_Load_Event(Event_Component* ec)
	{
		auto gCoordinator = *SXNGN::Database::get_coordinator();
		auto user_input_state = User_Input_State::get_instance();
		std::ifstream i("world.json");
		json j;
		try 
		{
			while (i >> j)
			{
				//std::cout << j.dump(4) << std::endl;
				auto extern_entity = gCoordinator.JSON_To_Entity(j);
				gCoordinator.Dump_Spaced_Entity_To_ECS(extern_entity);
			}
		}
		catch (const std::exception& exc)
		{
			std::cerr << exc.what();
		}
		Event_Component load_game_state_change;
		load_game_state_change.e.common.type = EventType::STATE_CHANGE;
		load_game_state_change.e.state_change.new_states.push_front(ComponentTypeEnum::MAIN_GAME_STATE);
		load_game_state_change.e.state_change.states_to_remove.push_front(ComponentTypeEnum::MAIN_MENU_STATE);
		Entity event_entity = Entity_Builder_Utils::Create_Event(gCoordinator, ComponentTypeEnum::CORE_BG_GAME_STATE, load_game_state_change, "Load Game State Change");
	}

	void Event_System::Handle_Mouse_Wheel_Event(Event_Component* ec)
	{
		auto cached_input_state = User_Input_State::get_instance();
		if (cached_input_state->selected_entities.size() == 0)
		{
			SXNGN::Database::modify_scale(ec->e.mouse_wheel_event.y_);
			std::cout << "Scale now: " << SXNGN::Database::get_scale() << std::endl;
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
				auto move_location = gCoordinator.GetComponentReadOnly(ec->e.order_event.clicked_entities.at(0), ComponentTypeEnum::COLLISION);
				if (!move_location)
				{
					return;
				}
				const Collisionable* collisionable = static_cast<const Collisionable*>(move_location);
				walk_chunk.location_.x = collisionable->collision_box_.x;
				walk_chunk.location_.y = collisionable->collision_box_.y;
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