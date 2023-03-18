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
			auto party_data = gCoordinator.CheckOutComponent(entity_actable, ComponentTypeEnum::PARTY);
			if (party_data)
			{
				ECS_Utils::update_pace();
				Party* party_ptr = static_cast<Party*>(party_data);


				//Handle Location in World

				auto world_map_uuid = gCoordinator.getUUID(SXNGN::WORLD_MAP);
				auto world_map_data = gCoordinator.GetComponentReadOnly(gCoordinator.GetEntityFromUUID(world_map_uuid), ComponentTypeEnum::WORLD_MAP);
				const WorldMap* worldmap_ptr = static_cast<const WorldMap*>(world_map_data);
				
				auto location_data = gCoordinator.GetComponentReadOnly(entity_actable, ComponentTypeEnum::LOCATION);
				const Location* location_ptr = static_cast<const Location*>(location_data);

				int world_map_grid_x = round(location_ptr->m_pos_x_m_ * SXNGN::OVERWORLD_GRIDS_PER_PIXEL);
				int world_map_grid_y = round(location_ptr->m_pos_y_m_ * SXNGN::OVERWORLD_GRIDS_PER_PIXEL);

				std::vector<sole::uuid> party_map_location_uuids;
				if (worldmap_ptr->world_locations_.size() > world_map_grid_x)
				{
					if (worldmap_ptr->world_locations_.at(world_map_grid_x).size() > world_map_grid_y)
					{
						party_map_location_uuids = worldmap_ptr->world_locations_[world_map_grid_x][world_map_grid_y];
					}
				}
				party_ptr->world_location_ids_ = party_map_location_uuids;
				

				// Handle Pace
			
				if (party_ptr->lost_counter_ > 0.0)
				{
					gCoordinator.setSetting(OVERWORLD_LOST, 1.0);
				}
				else
				{
					gCoordinator.setSetting(OVERWORLD_LOST, 0.0);
				}

				auto pace_setting = gCoordinator.getSetting(SXNGN::OVERWORLD_PACE_M_S);
				auto pace_go = gCoordinator.getSetting(SXNGN::OVERWORLD_GO);

				if (pace_setting.second && pace_go.second)
				{
					auto pace_m_s = pace_setting.first * pace_go.first;
					auto pace_value_label = ui_single->string_to_ui_map_["OVERWORLD_label_pace"];
					std::string pace_display = "";

						
					std::ostringstream oss;
					
					
					//if on the move, substract stamina, then health
					if (pace_go.first)
					{
						auto calories_per_km = 25 * party_ptr->hands_;
						
						oss << std::fixed << std::setprecision(2) << pace_m_s;
						auto dist_traveled_km = pace_m_s * dt * OVERWORLD_MULTIPLIER / 1000.0;
						if (party_ptr->lost_counter_ > 0.0)
						{
							party_ptr->lost_counter_ -= dist_traveled_km;
							if (party_ptr->lost_counter_ <= 0.0)
							{
								party_ptr->lost_counter_ = 0.0;
								ECS_Utils::update_pace();
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
					else if((party_ptr->stamina_ < party_ptr->stamina_max_  || party_ptr->sick_counter_ > 0.0)&& party_ptr->inventory_[ItemType::FOOD] >= 0.0)
					{
						double recharge_calories_per_minute = SXNGN::PARTY_RECHARGE_CALORIES_PER_MINUTE;
						double recharge_calories_per_second = recharge_calories_per_minute / OVERWORLD_MULTIPLIER;
						
						double calories_recharged = dt * OVERWORLD_MULTIPLIER * recharge_calories_per_second;
						double food_used = FOOD_UNITS_PER_CALORIES * calories_recharged;
						if (party_ptr->sick_counter_ > 0.0)
						{
							
							calories_recharged = calories_recharged * PARTY_SICK_STAMINA_RECOVER_PENALTY;
							double sick_time_recovered_s = dt * OVERWORLD_MULTIPLIER;
							party_ptr->sick_counter_ -= (sick_time_recovered_s / 60.0);
							if (party_ptr->sick_counter_ < 0)
							{
								party_ptr->sick_counter_ = 0.0;
							}
						}
						
						party_ptr->stamina_ += calories_recharged;
						
						party_ptr->remove_item(ItemType::FOOD, food_used);

						if (party_ptr->stamina_ >= party_ptr->stamina_max_)
						{
							party_ptr->stamina_ = party_ptr->stamina_max_;
						}
					}

					auto str = oss.str();
					snprintf(pace_value_label->label_->text, 100, "%s", str.data());
				}


				//Update UI for progress bars
				auto stamina_progress_bar = ui_single->string_to_ui_map_["OVERWORLD_progress_stamina"];
				stamina_progress_bar->progressbar_->value = party_ptr->stamina_;
				stamina_progress_bar->progressbar_->max_value = party_ptr->stamina_max_;
				auto health_progress_bar = ui_single->string_to_ui_map_["OVERWORLD_progress_health"];
				health_progress_bar->progressbar_->value = party_ptr->health_;
				health_progress_bar->progressbar_->max_value = party_ptr->health_;
				auto food_progress_bar = ui_single->string_to_ui_map_["OVERWORLD_progress_food"];
				food_progress_bar->progressbar_->value = party_ptr->inventory_[ItemType::FOOD];
				food_progress_bar->progressbar_->max_value = party_ptr->food_max_;
				auto weight_progress_bar = ui_single->string_to_ui_map_["OVERWORLD_progress_encumber"];
				weight_progress_bar->progressbar_->value = party_ptr->encumbrance_kg_;
				weight_progress_bar->progressbar_->max_value = party_ptr->weight_capacity_kg_;
				auto lost_progress_bar = ui_single->string_to_ui_map_["OVERWORLD_progress_lost"];
				lost_progress_bar->progressbar_->value = party_ptr->lost_counter_;
				lost_progress_bar->progressbar_->max_value = party_ptr->lost_counter_max_;
				auto sick_progress_bar = ui_single->string_to_ui_map_["OVERWORLD_progress_sick"];
				sick_progress_bar->progressbar_->value = party_ptr->sick_counter_;
				sick_progress_bar->progressbar_->max_value = party_ptr->sick_counter_max_;

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
