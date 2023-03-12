#include <ECS/Systems/DirectorSystem.hpp>
#include <ECS/Core/Coordinator.hpp>
#include <ECS/Components/Director.hpp>
#include <ECS/Utilities/ECS_Utils.hpp>




namespace SXNGN::ECS
{

	void Director_System::Init()
	{
		std::srand(std::time(nullptr));

		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Director_System Init");
		
	}

	void Director_System::Update(double dt)
	{
		auto ui_single = UICollectionSingleton::get_instance();
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
			auto director_data = gCoordinator.CheckOutComponent(entity_actable, ComponentTypeEnum::DIRECTOR);
			if (director_data)
			{
				Director* director_ptr = static_cast<Director*>(director_data);

				if (!director_ptr->has_event_table_)
				{
					auto new_event_table = GenerateEventTable();
					director_ptr->event_table_ = new_event_table;
					director_ptr->has_event_table_ = true;
				}

				int inc = dt * OVERWORLD_MULTIPLIER;
				director_ptr->game_clock_ += inc;
				auto date_value = ui_single->string_to_ui_map_["OVERWORLD_date_value"];
				std::string day_format = "%Y-%m-%d";  // define the format string
				char buffer[80];
				std::strftime(buffer, sizeof(buffer), day_format.c_str(), std::localtime(&director_ptr->game_clock_));  // format the time as a string
				snprintf(date_value->label_->text, 100, "%s", buffer);

				auto time_value = ui_single->string_to_ui_map_["OVERWORLD_time_value"];
				std::string time_format = "%H:%M";  // define the format string
				std::strftime(buffer, sizeof(buffer), time_format.c_str(), std::localtime(&director_ptr->game_clock_));  // format the time as a string
				snprintf(time_value->label_->text, 100, "%s", buffer);

				auto pace_go = gCoordinator.getSetting(SXNGN::OVERWORLD_GO);
				if (pace_go.first > 0.0)
				{
					director_ptr->event_tick_s_ += dt;
				}
				
				if (director_ptr->event_tick_s_ >= director_ptr->event_gauge_s_)
				{
					director_ptr->event_tick_s_ = 0.0;
					auto generated_event = director_ptr->event_table_.generate_event(&director_ptr->event_table_);
					std::cout << "Generating event " << generated_event->to_std_string() << std::endl;
					Event_Component* event_component = new Event_Component();
					SXNGN_Party party_event;
					party_event.party_event_type = generated_event->value;
					party_event.party_id = gCoordinator.getUUID(SXNGN::OVERWORLD_PLAYER_UUID);
					party_event.severity = EventSeverity::MILD;
					event_component->e.party_event = party_event;
					event_component->e.common.type = EventType::PARTY;
					Entity event_entity = gCoordinator.CreateEntity();
					gCoordinator.AddComponent(event_entity, event_component);
					gCoordinator.AddComponent(event_entity, Create_Gamestate_Component_from_Enum(ComponentTypeEnum::OVERWORLD_STATE));
				}
			}
			gCoordinator.CheckInComponent(entity_actable, ComponentTypeEnum::DIRECTOR);
			
			
		}
	}

	DropEntry<PartyEventType> Director_System::GenerateEventTable()
	{
		
		DropEntry<PartyEventType> event_table;

		DropEntry<PartyEventType> none_event;
		none_event.weight = 51;
		none_event.reoccurance_penalty = 10;
		none_event.accumulation = 50;
		none_event.value = PartyEventType::NONE;

		SXNGN::ECS::DropEntry<PartyEventType> good_events;
		good_events.weight = 0;
		good_events.accumulation = 0;
		good_events.value = PartyEventType::GOOD;
		good_events.children.push_back(none_event);
		DropEntry<PartyEventType> bad_events;
		bad_events.weight = 50;
		bad_events.value = PartyEventType::BAD;
		//bad_events.children.push_back(none_event);
		DropEntry<PartyEventType> neutral_events;
		neutral_events.weight = 0;
		neutral_events.accumulation = 0;
		neutral_events.value = PartyEventType::NEUTRAL;
		neutral_events.children.push_back(none_event);

		for (int i = PartyEventType::BAD + 1; i != PartyEventType::GOOD; i++)
		{
			PartyEventType event_type = static_cast<PartyEventType>(i);
			DropEntry<PartyEventType> event_entry;
			event_entry.weight = i * 3;
			event_entry.value = event_type;
			bad_events.children.push_back(event_entry);
		}
		for (int i = PartyEventType::GOOD + 1; i != PartyEventType::NEUTRAL; i++)
		{
			PartyEventType event_type = static_cast<PartyEventType>(i);
			DropEntry<PartyEventType> event_entry;
			event_entry.weight = 10;
			event_entry.value = event_type;
			good_events.children.push_back(event_entry);
		}
		for (int i = PartyEventType::NEUTRAL + 1; i != PartyEventType::NONE; i++)
		{
			PartyEventType event_type = static_cast<PartyEventType>(i);
			DropEntry<PartyEventType> event_entry;
			event_entry.weight = 10;
			event_entry.value = event_type;
			neutral_events.children.push_back(event_entry);
		}


		event_table.children.push_back(bad_events);
		event_table.children.push_back(good_events);
		event_table.children.push_back(neutral_events);
		event_table.children.push_back(none_event);
		event_table.weight = 1;

		event_table.print_event_table(event_table);

		return event_table;
	}
			

		
		
		

}
