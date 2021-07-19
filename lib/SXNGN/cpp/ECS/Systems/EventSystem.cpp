
#include<Collision.h>
#include <ECS/Core/Coordinator.hpp>
#include <Database.h>
#include <ECS/Systems/EventSystem.hpp>
#include <ECS/Components/EventComponent.hpp>

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
				case EventType::SELECTION:
				{
					//todo max number of selected entities
					SDL_LogInfo(1, "Event_System::Update:: Got Selection Event");
					auto user_input_state = User_Input_State::get_instance();
					if (event_ptr->e.select_event.additive)
					{
						for (auto entity : event_ptr->e.select_event.clicked_entities)
						{
							user_input_state->selected_entities[entity] = true;
						}
					}
					else if (event_ptr->e.select_event.subtractive)
					{
						for (auto entity : event_ptr->e.select_event.clicked_entities)
						{
							user_input_state->selected_entities[entity] = false;
						}
					}
					else if (event_ptr->e.select_event.enqueue)
					{
						//todo job queueing system
					}
					else
					{
						std::array<bool, MAX_ENTITIES> selected_entities{ false };
						user_input_state->selected_entities = selected_entities;
						for (auto entity : event_ptr->e.select_event.clicked_entities)
						{
							user_input_state->selected_entities[entity] = true;
						}
					}

					//list all the entities that are now selected
					SDL_LogInfo(1, "Event_System::Update::Selection --- selected entity list:");
					for (int i = 0; i < user_input_state->selected_entities.size(); i++)
					{
						if (user_input_state->selected_entities[i])
						{
							SDL_LogInfo(1, "%d", i);
						}
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

	void Event_System::Handle_Event(Event_Component* ec)
	{
		

	}

}