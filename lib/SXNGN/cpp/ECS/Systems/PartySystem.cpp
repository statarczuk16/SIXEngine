#include <ECS/Systems/PartySystem.hpp>
#include <ECS/Core/Coordinator.hpp>
#include <ECS/Components/Party.hpp>
#include <ECS/Utilities/ECS_Utils.hpp>




namespace SXNGN::ECS::A 
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
					//if on the move, substract stamina, then health
					if (pace_go.second)
					{
						auto calories_per_km = 50 * party_ptr->hands_;

						auto pace_m_s = pace_setting.first * pace_go.first;
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
					else
					{
						double calories_per_food_unit = 200;


					}
					
					
				}
				gCoordinator.setSetting(SXNGN::PARTY_STAMINA, party_ptr->stamina_);
				gCoordinator.setSetting(SXNGN::PARTY_FOOD, party_ptr->food_);
				gCoordinator.setSetting(SXNGN::PARTY_HEALTH, party_ptr->health_);

			}
			gCoordinator.CheckInComponent(entity_actable, ComponentTypeEnum::PARTY);
			
			
		}
	}
			

		
		
		

}
