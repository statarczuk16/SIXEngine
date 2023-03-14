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
				Party* party_ptr = static_cast<Party*>(party_data);
			
				auto pace_setting = gCoordinator.getSetting(SXNGN::OVERWORLD_PACE_M_S);
				auto pace_go = gCoordinator.getSetting(SXNGN::OVERWORLD_GO);
				if (pace_setting.second && pace_go.second)
				{
					auto pace_m_s = pace_setting.first * pace_go.first;
					auto pace_value_label = ui_single->string_to_ui_map_["OVERWORLD_label_pace"];

					snprintf(pace_value_label->label_->text, 100, "%g", pace_m_s);
					//if on the move, substract stamina, then health
					if (pace_go.first)
					{
						auto calories_per_km = 25 * party_ptr->hands_;
						
						
						auto dist_traveled_km = pace_m_s * dt * OVERWORLD_MULTIPLIER / 1000.0;
						auto stamina_upkeep = calories_per_km * dist_traveled_km;
						party_ptr->stamina_ -= stamina_upkeep;
						if (party_ptr->stamina_ <= 0.0)
						{
							party_ptr->stamina_ = 0.0;
							party_ptr->health_ -= stamina_upkeep;
						}
					}
					//else recharge using food
					else if(party_ptr->stamina_ < party_ptr->stamina_max_ && party_ptr->inventory_[ItemType::FOOD] >= 0.0)
					{
						double recharge_calories_per_minute = 5.0;
						double recharge_calories_per_second = recharge_calories_per_minute / OVERWORLD_MULTIPLIER;
						double food_units_per_calorie = 0.1;
						double calories_recharged = dt * OVERWORLD_MULTIPLIER * recharge_calories_per_second;
						party_ptr->stamina_ += calories_recharged;
						double food_used = food_units_per_calorie * calories_recharged;
						party_ptr->remove_item(ItemType::FOOD, food_used);

						if (party_ptr->stamina_ >= party_ptr->stamina_max_)
						{
							party_ptr->stamina_ = party_ptr->stamina_max_;
						}
					}
				}

				auto stamina_progress_bar = ui_single->string_to_ui_map_["OVERWORLD_progress_stamina"];
				stamina_progress_bar->progressbar_->value = party_ptr->stamina_;
				stamina_progress_bar->progressbar_->max_value = party_ptr->stamina_max_;
				auto health_progress_bar = ui_single->string_to_ui_map_["OVERWORLD_progress_health"];
				health_progress_bar->progressbar_->value = party_ptr->health_;
				health_progress_bar->progressbar_->max_value = party_ptr->health_;
				auto food_progress_bar = ui_single->string_to_ui_map_["OVERWORLD_progress_food"];
				food_progress_bar->progressbar_->value = party_ptr->inventory_[ItemType::FOOD];
				food_progress_bar->progressbar_->max_value = party_ptr->food_max_;

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
				
				

			}
			gCoordinator.CheckInComponent(entity_actable, ComponentTypeEnum::PARTY);
			
			
		}
	}
			

		
		
		

}
