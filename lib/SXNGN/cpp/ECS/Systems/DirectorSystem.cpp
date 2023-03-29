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
				double traversal_cost_penalty_m_s_ = 0.0;
				auto ui = UICollectionSingleton::get_instance();
				Entity settlement_entity = -1;
				if (overworld_player_uuid != SXNGN::BAD_UUID)
				{
					auto overworld_player_entity = gCoordinator.GetEntityFromUUID(overworld_player_uuid);
					auto party_component = gCoordinator.CheckOutComponent(overworld_player_entity, ComponentTypeEnum::PARTY);
					if (party_component)
					{
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
								settlement_entity = gCoordinator.GetEntityFromUUID(id);
								at_settlement = true;
								auto settlement_name_label = ui->string_to_ui_map_["OVERWORLD_settlement_name"];
								snprintf(settlement_name_label->label_->text, KISS_MAX_LABEL, ptr->location_name_.data());
							}
							traversal_cost_penalty_m_s_ += ptr->traversal_cost_m_s_;
						}

						gCoordinator.CheckInComponent(overworld_player_entity, ComponentTypeEnum::PARTY);
					}
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
					auto overworld_player_entity = gCoordinator.GetEntityFromUUID(overworld_player_uuid);

						
					

					std::function<void()> trade_with_inv = [overworld_player_entity, settlement_entity]()
					{
						TradeHelper* trade_helper = new TradeHelper();
						trade_helper->left_trader = overworld_player_entity;
						trade_helper->right_trader = settlement_entity;
						trade_helper->left_is_player = true;
						auto trading_window = UserInputUtils::create_trading_menu(nullptr, "Trading", "Buy some stuff!", UILayer::BOTTOM, trade_helper);
						auto ui = UICollectionSingleton::get_instance();
						ui->add_ui_element(ComponentTypeEnum::OVERWORLD_STATE, trading_window);
					};
					//auto trade_here = std::bind(trade_with_inv, trade_helper);

					auto trade_button = ui->string_to_ui_map_["OVERWORLD_trade_button"];
					trade_button->button_->enabled = true;
					trade_button->callback_functions_.clear();
					trade_button->callback_functions_.push_back(trade_with_inv);



					
					
				}
				else
				{
					auto settlement_name_label = ui->string_to_ui_map_["OVERWORLD_settlement_name"];
					snprintf(settlement_name_label->label_->text, KISS_MAX_LABEL,"None");
					auto trade_button = ui->string_to_ui_map_["OVERWORLD_trade_button"];
					trade_button->button_->enabled = false;
				}
				gCoordinator.setSetting(OVERWORLD_PACE_PENALTY_M_S, traversal_cost_penalty_m_s_);



				auto pace_go = gCoordinator.getSetting(SXNGN::OVERWORLD_PACE_TOTAL_M_S);
				bool moving = pace_go.first > 0.0;
				bool scavenging = false;
				bool towning = false;
				if (moving || scavenging || towning)
				{
					director_ptr->event_tick_s_ += dt;
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
						PruneEvents(director_ptr, party_ptr, at_settlement, at_ruins, moving);


						gCoordinator.CheckInComponent(overworld_player_entity, ComponentTypeEnum::PARTY);
					}
					
					auto generated_event = director_ptr->event_table_.generate_event(&director_ptr->event_table_);
					std::cout << "Generated event " << generated_event->to_std_string() << std::endl;
					director_ptr->event_table_.print_event_table(director_ptr->event_table_);
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

	void Director_System::PruneEvents(Director* director_ptr, Party* party_ptr, bool at_settlement, bool at_ruins, bool party_moving)
	{
		if (party_ptr->inventory_[ItemType::FOOTWEAR] < 1)
		{
			DropEntry<PartyEventType>* event_ptr = FindEventByType(director_ptr, PartyEventType::ROAD_BAD_BOOTS);
			event_ptr->weight = 0;
		}
		if (party_ptr->sick_counter_s_ > 0.0)
		{
			DropEntry<PartyEventType>* event_ptr = FindEventByType(director_ptr, PartyEventType::ROAD_BAD_SICK);
			event_ptr->weight = 0;
		}
		if (party_ptr->lost_counter_ > 0.0)
		{
			DropEntry<PartyEventType>* event_ptr = FindEventByType(director_ptr, PartyEventType::ROAD_BAD_LOST);
			event_ptr->weight = 0;
		}
		if (party_ptr->weather_counter_s_ > 0.0)
		{
			DropEntry<PartyEventType>* event_ptr = FindEventByType(director_ptr, PartyEventType::ANY_BAD_WEATHER);
			event_ptr->weight = 0;
		}
		if (at_settlement)
		{
			DropEntry<PartyEventType>* event_ptr = FindEventByType(director_ptr, PartyEventType::SETTLEMENT);
			event_ptr->weight = 100;
		}
		else
		{
			DropEntry<PartyEventType>* event_ptr = FindEventByType(director_ptr, PartyEventType::SETTLEMENT);
			event_ptr->weight = 0;
		}

		if (at_ruins)
		{
			DropEntry<PartyEventType>* event_ptr = FindEventByType(director_ptr, PartyEventType::RUINS);
			event_ptr->weight = 100;
		}
		else
		{
			DropEntry<PartyEventType>* event_ptr = FindEventByType(director_ptr, PartyEventType::RUINS);
			event_ptr->weight = 0;
		}

		if (party_moving)
		{
			DropEntry<PartyEventType>* event_ptr = FindEventByType(director_ptr, PartyEventType::ROAD);
			event_ptr->weight = 100;
		}
		else
		{
			DropEntry<PartyEventType>* event_ptr = FindEventByType(director_ptr, PartyEventType::ROAD);
			event_ptr->weight = 0;
		}

		DropEntry<PartyEventType>* event_ptr = FindEventByType(director_ptr, PartyEventType::ANY);
		event_ptr->weight = 100;
	}

	DropEntry<PartyEventType> Director_System::GenerateEventTable()
	{
		
		DropEntry<PartyEventType> event_table;

		DropEntry<PartyEventType> none_event;
		none_event.weight = 51;
		none_event.reoccurance_penalty = 10;
		none_event.accumulation = 50;
		none_event.value = PartyEventType::NONE;



		

		

		for (int i = PartyEventType::NONE + 1; i != PartyEventType::ANY_END; i++)
		{
			PartyEventType event_type = static_cast<PartyEventType>(i);
			
			if (i == ROAD)
			{
				DropEntry<PartyEventType> road_event_tree;
				road_event_tree.value = PartyEventType::ROAD;
				road_event_tree.weight = 0;
				road_event_tree.accumulation = 0;
				event_table.children.push_back(road_event_tree);
			}
			else if (i > ROAD_START && i < ROAD_END)
			{
				DropEntry<PartyEventType>* road_event_tree = event_table.find_event_by_type(PartyEventType::ROAD);

				if (i == ROAD_BAD || i == ROAD_NEUTRAL || i == ROAD_GOOD)
				{
					DropEntry<PartyEventType> event_tree;
					event_tree.value = event_type;
					road_event_tree->children.push_back(event_tree);
				}
				else if (i > ROAD_BAD && i < ROAD_GOOD)
				{
					DropEntry<PartyEventType>* et = event_table.find_event_by_type(PartyEventType::ROAD_BAD);
					DropEntry<PartyEventType> pe;
					pe.value = event_type;
					pe.weight = 0;
					pe.accumulation = 10;
					et->children.push_back(pe);
				}
				else if (i > ROAD_GOOD && i < ROAD_NEUTRAL)
				{
					DropEntry<PartyEventType> pe;
					pe.value = event_type;
					pe.weight = 0;
					pe.accumulation = 10;
					DropEntry<PartyEventType>* et = event_table.find_event_by_type(PartyEventType::ROAD_GOOD);
					et->children.push_back(pe);
				}
				else if (i > ROAD_NEUTRAL && i < ROAD_END)
				{
					DropEntry<PartyEventType> pe;
					pe.value = event_type;
					pe.weight = 0;
					pe.accumulation = 10;
					DropEntry<PartyEventType>* et = event_table.find_event_by_type(PartyEventType::ROAD_NEUTRAL);
					et->children.push_back(pe);
				}
				
			}
			else if (i == RUINS)
			{
				DropEntry<PartyEventType> event_tree;
				event_tree.value = PartyEventType::RUINS;
				event_tree.weight = 0;
				event_tree.accumulation = 0;
				event_table.children.push_back(event_tree);
			}
			else if (i > RUINS_START && i < RUINS_END)
			{
				
				DropEntry<PartyEventType>* ruins_event_tree = event_table.find_event_by_type(PartyEventType::RUINS);

				if (i == RUINS_BAD || i == RUINS_NEUTRAL || i == RUINS_GOOD)
				{
					DropEntry<PartyEventType> event_tree;
					event_tree.value = event_type;
					ruins_event_tree->children.push_back(event_tree);
				}
				else if (i > RUINS_BAD && i < RUINS_GOOD)
				{
					DropEntry<PartyEventType> pe;
					pe.value = event_type;
					pe.weight = 0;
					pe.accumulation = 10;
					DropEntry<PartyEventType>* et = event_table.find_event_by_type(PartyEventType::RUINS_BAD);
					et->children.push_back(pe);
				}
				else if (i > RUINS_GOOD && i < RUINS_NEUTRAL)
				{
					DropEntry<PartyEventType> pe;
					pe.value = event_type;
					pe.weight = 0;
					pe.accumulation = 10;
					DropEntry<PartyEventType>* et = event_table.find_event_by_type(PartyEventType::RUINS_GOOD);
					et->children.push_back(pe);
				}
				else if (i > RUINS_NEUTRAL && i < RUINS_END)
				{
					DropEntry<PartyEventType> pe;
					pe.value = event_type;
					pe.weight = 0;
					pe.accumulation = 10;
					DropEntry<PartyEventType>* et = event_table.find_event_by_type(PartyEventType::RUINS_NEUTRAL);
					et->children.push_back(pe);
				}
				
			}
			else if (i == SETTLEMENT)
			{
				DropEntry<PartyEventType> event_tree;
				event_tree.value = PartyEventType::SETTLEMENT;
				event_tree.weight = 0;
				event_tree.accumulation = 0;
				event_table.children.push_back(event_tree);
			}
			else if (i > SETTLEMENT_START && i < SETTLEMENT_END)
			{
				DropEntry<PartyEventType>* settlement_event_tree = event_table.find_event_by_type(PartyEventType::SETTLEMENT);
				if (i == SETTLEMENT_BAD || i == SETTLEMENT_NEUTRAL || i == SETTLEMENT_GOOD)
				{
					DropEntry<PartyEventType> event_tree;
					event_tree.value = event_type;
					settlement_event_tree->children.push_back(event_tree);
				}
				else if (i > SETTLEMENT_BAD && i < SETTLEMENT_GOOD)
				{
					DropEntry<PartyEventType> pe;
					pe.value = event_type;
					pe.weight = 0;
					pe.accumulation = 10;
					DropEntry<PartyEventType>* et = event_table.find_event_by_type(PartyEventType::SETTLEMENT_BAD);
					et->children.push_back(pe);
				}
				else if (i > SETTLEMENT_GOOD && i < SETTLEMENT_NEUTRAL)
				{
					DropEntry<PartyEventType> pe;
					pe.value = event_type;
					pe.weight = 0;
					pe.accumulation = 10;
					DropEntry<PartyEventType>* et = event_table.find_event_by_type(PartyEventType::SETTLEMENT_GOOD);
					et->children.push_back(pe);
				}
				else if (i > SETTLEMENT_NEUTRAL && i < SETTLEMENT_END)
				{
					DropEntry<PartyEventType> pe;
					pe.value = event_type;
					pe.weight = 0;
					pe.accumulation = 10;
					DropEntry<PartyEventType>* et = event_table.find_event_by_type(PartyEventType::SETTLEMENT_NEUTRAL);
					et->children.push_back(pe);
				}
				
			}
			else if (i == ANY)
			{
			DropEntry<PartyEventType> event_tree;
			event_tree.value = PartyEventType::ANY;
			event_tree.weight = 0;
			event_tree.accumulation = 0;
			event_table.children.push_back(event_tree);
			}
			else if (i > ANY_START && i < ANY_END)
			{
				DropEntry<PartyEventType>* any_event_tree = event_table.find_event_by_type(PartyEventType::ANY);
				if (i == ANY_BAD || i == ANY_NEUTRAL || i == ANY_GOOD)
				{
					DropEntry<PartyEventType> event_tree;
					event_tree.value = event_type;
					any_event_tree->children.push_back(event_tree);
				}
				else if (i > ANY_BAD && i < ANY_GOOD)
				{
					DropEntry<PartyEventType> pe;
					pe.value = event_type;
					pe.weight = 0;
					pe.accumulation = 10;
					DropEntry<PartyEventType>* et = event_table.find_event_by_type(PartyEventType::ANY_BAD);
					et->children.push_back(pe);
				}
				else if (i > ANY_GOOD && i < ANY_NEUTRAL)
				{
					DropEntry<PartyEventType> pe;
					pe.value = event_type;
					pe.weight = 0;
					pe.accumulation = 10;
					DropEntry<PartyEventType>* et = event_table.find_event_by_type(PartyEventType::ANY_GOOD);
					et->children.push_back(pe);
				}
				else if (i > ANY_NEUTRAL && i < ANY_END)
				{
					DropEntry<PartyEventType> pe;
					pe.value = event_type;
					pe.weight = 0;
					pe.accumulation = 10;
					DropEntry<PartyEventType>* et = event_table.find_event_by_type(PartyEventType::ANY_NEUTRAL);
					et->children.push_back(pe);
				}
				
			}
		}


		//disable robber for until combat implemented
		DropEntry<PartyEventType>* event_ptr = event_table.find_event_by_type(PartyEventType::ROAD_BAD_ROBBER);
		event_ptr->max_weight = 0.0;
		event_ptr->weight = 0.0;
	
		DropEntry<PartyEventType>* et = event_table.find_event_by_type(PartyEventType::ANY_NEUTRAL);
		et->children.push_back(none_event);

		et = event_table.find_event_by_type(PartyEventType::ROAD_NEUTRAL);
		et->children.push_back(none_event);

		et = event_table.find_event_by_type(PartyEventType::SETTLEMENT_NEUTRAL);
		et->children.push_back(none_event);

		et = event_table.find_event_by_type(PartyEventType::RUINS_NEUTRAL);
		et->children.push_back(none_event);

	
		event_table.print_event_table(event_table);

		return event_table;
	}
			

	
	
		

}


