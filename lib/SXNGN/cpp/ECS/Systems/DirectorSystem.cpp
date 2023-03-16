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

				//Update the date and time
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

				//Get data about overworld locations player can touch
				auto overworld_player_uuid = gCoordinator.getUUID(SXNGN::OVERWORLD_PLAYER_UUID);
				bool at_settlement = false;
				bool at_ruins = false;
				auto ui = UICollectionSingleton::get_instance();
				if (overworld_player_uuid != SXNGN::BAD_UUID)
				{
					auto overworld_player_entity = gCoordinator.GetEntityFromUUID(overworld_player_uuid);
					auto party_component = gCoordinator.CheckOutComponent(overworld_player_entity, ComponentTypeEnum::PARTY);
					auto party_ptr = static_cast<Party*>(party_component);
					for (auto id : party_ptr->world_location_ids_)
					{
						auto world_location_data = gCoordinator.GetComponentReadOnly(gCoordinator.GetEntityFromUUID(id), ComponentTypeEnum::WORLD_LOCATION);
						const WorldLocation* ptr = static_cast<const WorldLocation*>(world_location_data);
						if (ptr->has_ruins_)
						{
							at_ruins = true;
							auto ruins_name_label = ui->string_to_ui_map_["OVERWORLD_ruins_name"];
							snprintf(ruins_name_label->label_->text, KISS_MAX_LABEL, ptr->location_name_.data());

						}
						if (ptr->has_settlement_)
						{
							at_settlement = true;
							auto settlement_name_label = ui->string_to_ui_map_["OVERWORLD_settlement_name"];
							snprintf(settlement_name_label->label_->text, KISS_MAX_LABEL, ptr->location_name_.data());
						}
					}

					gCoordinator.CheckInComponent(overworld_player_entity, ComponentTypeEnum::PARTY);
				}
				if (at_ruins)
				{
					auto scavenge_button = ui->string_to_ui_map_["OVERWORLD_scavenge_button"];
					scavenge_button->button_->enabled = true;
				}
				else
				{
					auto ruins_name_label = ui->string_to_ui_map_["OVERWORLD_ruins_name"];
					snprintf(ruins_name_label->label_->text, KISS_MAX_LABEL, "None");
					auto scavenge_button = ui->string_to_ui_map_["OVERWORLD_scavenge_button"];
					scavenge_button->button_->enabled = false;
				}

				if (at_settlement)
				{
					auto trade_button = ui->string_to_ui_map_["OVERWORLD_trade_button"];
					trade_button->button_->enabled = true;
				}
				else
				{
					auto settlement_name_label = ui->string_to_ui_map_["OVERWORLD_settlement_name"];
					snprintf(settlement_name_label->label_->text, KISS_MAX_LABEL,"None");
					auto trade_button = ui->string_to_ui_map_["OVERWORLD_trade_button"];
					trade_button->button_->enabled = false;
				}


				auto pace_go = gCoordinator.getSetting(SXNGN::OVERWORLD_GO);
				if (pace_go.first > 0.0)
				{
					director_ptr->event_tick_s_ += dt * 10;
				}
				
				if (director_ptr->event_tick_s_ >= director_ptr->event_gauge_s_)
				{
					director_ptr->event_tick_s_ = 0.0;


					auto overworld_player_uuid = gCoordinator.getUUID(SXNGN::OVERWORLD_PLAYER_UUID);
					if (overworld_player_uuid != SXNGN::BAD_UUID)
					{
						auto overworld_player_entity = gCoordinator.GetEntityFromUUID(overworld_player_uuid);
						auto party_component = gCoordinator.CheckOutComponent(overworld_player_entity, ComponentTypeEnum::PARTY);
						auto party_ptr = static_cast<Party*>(party_component);
						PruneEvents(director_ptr, party_ptr);


						gCoordinator.CheckInComponent(overworld_player_entity, ComponentTypeEnum::PARTY);
					}
					
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


	DropEntry<PartyEventType>* Director_System::FindEventByType(Director* director_ptr, PartyEventType search_val)
	{
		if (director_ptr->event_table_cache_.count(search_val))
		{
			if (director_ptr->event_table_cache_[search_val] != nullptr)
			{
				return director_ptr->event_table_cache_[search_val];
			}
		}
		DropEntry<PartyEventType>*  search_result = director_ptr->event_table_.find_event_by_type(search_val);
		if (search_result != nullptr)
		{
			if (search_result->value == search_val)
			{
				director_ptr->event_table_cache_[search_val] = search_result;
				return search_result;
			}
			else
			{
				SDL_LogError(1, "Bad Value in FindEventByType");
			}
		}
		return nullptr;
	}

	void Director_System::PruneEvents(Director* director_ptr, Party* party_ptr)
	{
		if (party_ptr->inventory_[ItemType::FOOTWEAR] < 1)
		{
			DropEntry<PartyEventType>* event_ptr = FindEventByType(director_ptr, PartyEventType::BAD_BOOTS);
			event_ptr->weight = 0;
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
