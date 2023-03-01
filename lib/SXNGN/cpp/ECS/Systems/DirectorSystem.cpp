#include <ECS/Systems/DirectorSystem.hpp>
#include <ECS/Core/Coordinator.hpp>
#include <ECS/Components/Party.hpp>
#include <ECS/Utilities/ECS_Utils.hpp>




namespace SXNGN::ECS::A 
{

	void Director_System::Init()
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Director_System Init");
		
	}

	void Director_System::Update(float dt)
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
			auto director_data = gCoordinator.CheckOutComponent(entity_actable, ComponentTypeEnum::DIRECTOR);
			if (director_data)
			{
				Director* director_ptr = static_cast<Director*>(director_data);
				

			}
			gCoordinator.CheckInComponent(entity_actable, ComponentTypeEnum::DIRECTOR);
			
			
		}
	}
			

		
		
		

}
