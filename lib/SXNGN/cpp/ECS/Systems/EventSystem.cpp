
#include<Collision.h>
#include <ECS/Core/Coordinator.hpp>
#include <Database.h>
#include <ECS/Systems/EventSystem.hpp>
#include <ECS/Components/EventComponent.hpp>

namespace SXNGN::ECS::System {

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
	//actable entities for user input system are UserInputCache (vector of sdl events)
	while (it_act != m_actable_entities.end())
	{
		auto const& entity_actable = *it_act;
		it_act++;

		//thread safe checkout of data
		auto check_out_event = gCoordinator.CheckOutComponent(entity_actable, ComponentTypeEnum::EVENT);
		if (check_out_event.first)
		{
			
			Components::Event_Component* evnt_ptr = static_cast<Components::Event_Component*>(check_out_event.first);
			//update position
			
			//check data back in
			gCoordinator.CheckInComponent(ComponentTypeEnum::MOVEABLE, entity_actable, std::move(check_out_event.second));
		}

	}
}

void Event_System::Handle_Event()
{
	
}

