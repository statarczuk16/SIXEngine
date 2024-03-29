#include <ECS/Systems/PartySystem.hpp>
#include <ECS/Core/Coordinator.hpp>
#include <ECS/Components/Party.hpp>
#include <ECS/Utilities/ECS_Utils.hpp>




namespace SXNGN::ECS
{

	void Party_System::Init()
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Party_System Init");
		
	}

	void Party_System::Update(float dt)
	{
		auto gCoordinator = *SXNGN::Database::get_coordinator();
		std::vector<Entity> entities_to_cleanup;

		//SDL_LogDebug(1, "User_Input_Begin ");
		auto it_act = m_actable_entities.begin();
		//actable entities for user input system are UserInputCache (vector of sdl events)
		auto total = m_actable_entities.size();
		
		auto ui_single = UICollectionSingleton::get_instance();
		while (it_act != m_actable_entities.end())
		{
			auto const& entity_actable = *it_act;

			

			it_act++;

			// Only player character affected by this system
			sole::uuid player_uuid = gCoordinator.getUUID(SXNGN::OVERWORLD_PLAYER_UUID);
			Entity player_id = gCoordinator.GetEntityFromUUID(player_uuid);
			if (entity_actable != player_id)
			{
				continue;
			}

			auto party_data = gCoordinator.CheckOutComponent(entity_actable, ComponentTypeEnum::PARTY);
			if (party_data)
			{
				ECS_Utils::update_pace();
				Party* party_ptr = static_cast<Party*>(party_data);
				std::ostringstream party_status_oss;
				party_ptr->update_player_ui();
				
				
				auto location_data = gCoordinator.GetComponentReadOnly(entity_actable, ComponentTypeEnum::LOCATION);
				const Location* location_ptr = static_cast<const Location*>(location_data);
				if (location_ptr)
				{
					//Handle Location in World
					auto world_map_uuid = gCoordinator.getUUID(SXNGN::WORLD_MAP);
					auto world_map_data = gCoordinator.GetComponentReadOnly(gCoordinator.GetEntityFromUUID(world_map_uuid), ComponentTypeEnum::WORLD_MAP);
					const WorldMap* worldmap_ptr = static_cast<const WorldMap*>(world_map_data);

					int world_map_grid_x = int(location_ptr->m_pos_x_m_ * SXNGN::METERS_TO_PIXELS_2 * SXNGN::OVERWORLD_MULTIPLIER * SXNGN::OVERWORLD_GRIDS_PER_METER);
					int world_map_grid_y = int(location_ptr->m_pos_y_m_ * SXNGN::METERS_TO_PIXELS_2 * SXNGN::OVERWORLD_MULTIPLIER * SXNGN::OVERWORLD_GRIDS_PER_METER);

					std::vector<sole::uuid> party_map_location_uuids;
					if (worldmap_ptr->world_locations_.size() > world_map_grid_y)
					{
						if (worldmap_ptr->world_locations_.at(world_map_grid_y).size() > world_map_grid_x)
						{
							party_map_location_uuids = worldmap_ptr->world_locations_[world_map_grid_y][world_map_grid_x];
						}
					}
					party_ptr->world_location_ids_ = party_map_location_uuids;
				}


				// Handle Pace
			
				if (party_ptr->lost_counter_ > 0.0)
				{
					gCoordinator.setSetting(OVERWORLD_LOST, 1.0);
					party_status_oss << "Lost for " << std::fixed << std::setprecision(2) << party_ptr->lost_counter_ << " KM" << std::endl;
				}
				else
				{
					gCoordinator.setSetting(OVERWORLD_LOST, 0.0);
				}

				double pace_base = PARTY_PACE_NOMINAL_M_S;

				
				auto pace_go = gCoordinator.getSetting(SXNGN::OVERWORLD_GO);
				auto pace_penalty_overworld = gCoordinator.getSetting(SXNGN::OVERWORLD_PACE_PENALTY_M_S);

				if (pace_go.second && pace_penalty_overworld.second)
				{
					double pace_m_s_real = party_ptr->pace_m_s_ - pace_penalty_overworld.first;
					if (pace_m_s_real <= 0) //dont walk backwards if overencumbered just stop
					{
						pace_m_s_real = 0.0;
					}
					pace_m_s_real = abs(pace_go.first) * pace_m_s_real;
					party_status_oss << "Traveling at " << std::fixed << std::setprecision(2) << pace_m_s_real << " M/S" << std::endl;
					party_status_oss << "  Terrain: " << std::fixed << std::setprecision(2) << pace_penalty_overworld.first << " M/S" << std::endl;
					party_status_oss << "  Weight: " << std::fixed << std::setprecision(2) << party_ptr->encumbrance_penalty_m_s_ << " M/S" << std::endl;
					gCoordinator.setSetting(OVERWORLD_PACE_M_S, abs(pace_m_s_real));
					double game_seconds_passed = dt * OVERWORLD_MULTIPLIER;
					auto pace_value_label = ui_single->string_to_ui_map_["OVERWORLD_label_pace"];

					auto pace_penalty_value_label = ui_single->string_to_ui_map_["OVERWORLD_inventory_penalty_value"];
					snprintf(pace_penalty_value_label->label_->text, 100, "%.1f M/S", party_ptr->encumbrance_penalty_m_s_);
					std::string pace_display = "";

						
					std::ostringstream oss;
					
					bool moving = abs(pace_go.first);
					bool sick = party_ptr->sick_counter_s_ > 0.0;
					bool lost = party_ptr->lost_counter_ > 0.0;
					bool have_food = party_ptr->inventory_[ItemType::FOOD] >= 0.0;
					bool max_stamina = party_ptr->stamina_ >= party_ptr->stamina_max_;
					bool sandstorm = party_ptr->weather_counter_s_ > 0.0;

					
					
					if (sick)
					{
						double sick_time_recovered_s = game_seconds_passed;
						party_status_oss << "Sick for: " << std::fixed << std::setprecision(2) << party_ptr->sick_counter_s_ * SXNGN::SECONDS_TO_HOURS << " Hours" << std::endl;
						if (!moving)
						{
							sick_time_recovered_s *= 2;
						}
						party_ptr->sick_counter_s_ -= sick_time_recovered_s;
						if (party_ptr->sick_counter_s_ < 0)
						{
							party_ptr->sick_counter_s_ = 0.0;
							party_ptr->sick_level_ = EventSeverity::UNKNOWN;
						}
					}

					
					if(sandstorm)
					{
						party_ptr->weather_counter_s_ -= game_seconds_passed;
						if (party_ptr->weather_counter_s_ < 0)
						{
							party_ptr->weather_counter_s_ = 0.0;
							party_ptr->weather_level_ = EventSeverity::UNKNOWN;
						}

						if (moving && sandstorm)
						{
							double hp_drain = 0.0;
							if (party_ptr->weather_level_ == EventSeverity::MILD)
							{
								hp_drain = game_seconds_passed * PARTY_WEATHER_HP_DRAIN_PER_SECOND_MILD;
							}
							else if (party_ptr->weather_level_ == EventSeverity::MEDIUM)
							{
								hp_drain = game_seconds_passed * PARTY_WEATHER_HP_DRAIN_PER_SECOND_MED;
							}
							else if (party_ptr->weather_level_ == EventSeverity::EXTREME)
							{
								hp_drain = game_seconds_passed * PARTY_WEATHER_HP_DRAIN_PER_SECOND_EXT;
							}
							party_ptr->health_ -= hp_drain;
							party_status_oss << "Losing health to sandstorm " << std::endl;
						}

					}

					std::string res = "";
					if(moving && !party_ptr->have_footwear(res))
					{
						double hp_drain = game_seconds_passed * PARTY_WEATHER_HP_DRAIN_PER_SECOND_MILD;
						party_ptr->health_ -= hp_drain;
						party_status_oss << "Losing health because no footwear " << std::endl;
					}

					//if on the move, substract stamina, then health
					if (moving)
					{
						auto calories_per_km = HANDS_BASE_CALORIES_PER_KM * party_ptr->hands_;
						
						oss << std::fixed << std::setprecision(2) << pace_m_s_real;
						auto dist_traveled_km = pace_m_s_real * game_seconds_passed * 0.001;
						if (lost)
						{
							party_ptr->lost_counter_ -= dist_traveled_km;
							if (party_ptr->lost_counter_ <= 0.0)
							{
								party_ptr->lost_counter_ = 0.0;
							}
							oss << "(LOST)";
						}
						auto stamina_upkeep = calories_per_km * dist_traveled_km;
						party_ptr->stamina_ -= stamina_upkeep;
						if (party_ptr->stamina_ <= 0.0)
						{
							party_ptr->stamina_ = 0.0;
							party_ptr->health_ -= stamina_upkeep;
						}
					}
					//else recharge using food
					else
					{
						if (!max_stamina && have_food)
						{
							double recharge_calories_per_minute = SXNGN::PARTY_RECHARGE_CALORIES_PER_MINUTE;
							double recharge_calories_per_second = recharge_calories_per_minute / OVERWORLD_MULTIPLIER;

							double calories_recharged = dt * OVERWORLD_MULTIPLIER * recharge_calories_per_second;
							double food_used = FOOD_UNITS_PER_CALORIES * calories_recharged;
							party_ptr->stamina_ += calories_recharged;

							party_ptr->remove_item(ItemType::FOOD, food_used);

							if (party_ptr->stamina_ >= party_ptr->stamina_max_)
							{
								party_ptr->stamina_ = party_ptr->stamina_max_;
							}
						}
					}

					oss << " M/S";
					auto str = oss.str();
					snprintf(pace_value_label->label_->text, 100, "%s", str.data());
				}

				auto party_str = party_status_oss.str();
				auto status_text = ui_single->string_to_ui_map_["OVERWORLD_status_text"];
				snprintf(status_text->label_->text, KISS_MAX_TEXT, "%s", party_str.data());
				//Update UI for progress bars
				auto stamina_progress_bar = ui_single->string_to_ui_map_["OVERWORLD_progress_stamina"];
				stamina_progress_bar->progressbar_->value = party_ptr->stamina_;
				stamina_progress_bar->progressbar_->max_value = party_ptr->stamina_max_;
				auto health_progress_bar = ui_single->string_to_ui_map_["OVERWORLD_progress_health"];
				health_progress_bar->progressbar_->value = party_ptr->health_;
				health_progress_bar->progressbar_->max_value = party_ptr->health_max_;
				//auto food_progress_bar = ui_single->string_to_ui_map_["OVERWORLD_progress_food"];
				//food_progress_bar->progressbar_->value = party_ptr->inventory_[ItemType::FOOD];
				//food_progress_bar->progressbar_->max_value = party_ptr->food_max_;
				
				auto lost_progress_bar = ui_single->string_to_ui_map_["OVERWORLD_progress_lost"];
				lost_progress_bar->progressbar_->value = party_ptr->lost_counter_ * 1000.0;
				lost_progress_bar->progressbar_->max_value = party_ptr->lost_counter_max_ * 1000.0;
				auto sick_progress_bar = ui_single->string_to_ui_map_["OVERWORLD_progress_sick"];
				sick_progress_bar->progressbar_->value = party_ptr->sick_counter_s_ / 60.0;
				sick_progress_bar->progressbar_->max_value = party_ptr->sick_counter_max_ / 60.0;
				auto weather_progress_bar = ui_single->string_to_ui_map_["OVERWORLD_progress_weather"];
				weather_progress_bar->progressbar_->value = party_ptr->weather_counter_s_ / 60.0;
				weather_progress_bar->progressbar_->max_value = party_ptr->weather_counter_max_ / 60.0;

				//Update UI for party inventory
				int column = 2;
				for (int i = ItemType::UNKNOWN + 1; i != ItemType::END; i++)
				{
					ItemType item_type = static_cast<ItemType>(i);
					std::string item_name_str = item_type_to_string()[item_type];
					std::string ui_label_name = "OVERWORLD_amount_" + item_name_str;
					auto label = ui_single->string_to_ui_map_[ui_label_name];
					if (party_ptr->inventory_.count(item_type))
					{
						double amount = party_ptr->inventory_[item_type];
						snprintf(label->label_->text, 100, "%.2f", amount);
						//label->label_->text[amount.size()] = '\0';
					}
				}
				std::string item_name_str = "hands";
				std::string ui_label_name = "OVERWORLD_amount_" + item_name_str;
				auto label = ui_single->string_to_ui_map_[ui_label_name];
				double amount = party_ptr->hands_;
				snprintf(label->label_->text, 100, "%.2f", amount);
				//label->label_->text[amount.size()] = '\0';
				

				
				

			}
			gCoordinator.CheckInComponent(entity_actable, ComponentTypeEnum::PARTY);
			
			
		}
	}
			

		
		
		

}
