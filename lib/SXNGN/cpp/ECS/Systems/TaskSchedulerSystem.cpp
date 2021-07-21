
#include<Collision.h>
#include <ECS/Core/Coordinator.hpp>
#include <Database.h>
#include <ECS/Systems/TaskSchedulerSystem.hpp>
#include <ECS/Components/EventComponent.hpp>
#include <ECS/Components/TaskWorkerComponent.hpp>

namespace SXNGN::ECS::A {

	void Task_Scheduler_System::Init()
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Task_Scheduler_System Init");
	}

	void Task_Scheduler_System::Update(float dt)
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
			auto check_out_event = gCoordinator.CheckOutComponent(entity_actable, ComponentTypeEnum::TASK);
			if (check_out_event)
			{

				Task_Component* task_ptr = static_cast<Task_Component*>(check_out_event);
				//act on event
				if (task_ptr->tasks_.empty())
				{
					SDL_LogDebug(1,"Task completed: Entity: %d : UUID %s",entity_actable, task_ptr->id_.base62().c_str());
					continue;
				}
				if (task_ptr && !task_ptr->scheduled_)
				{
					auto it_int = m_entities_of_interest.begin();
					//schedule the task
					std::map<Entity, Sint32> worker_candidate_map_;
					while (it_int != m_entities_of_interest.end())
					{
						auto const& entity_int = *it_int;
						it_int++;
						auto check_out_worker = gCoordinator.CheckOutComponent(entity_int, ComponentTypeEnum::TASK_WORKER);
						if (check_out_worker)
						{
							Task_Worker_Component* worker_ptr = static_cast<Task_Worker_Component*>(check_out_event);
							
							//if worker has the skill required to do the work the task current needs
							if (worker_ptr->skill_enable_[task_ptr->tasks_.at(0).skill_required_] == true)
							{
								worker_candidate_map_[entity_int] = -1;//is a candidate
							}
							
							auto moveable_check_out = gCoordinator.GetComponentReadOnly(entity_int, ComponentTypeEnum::MOVEABLE);
							const Moveable* moveable_ptr;
							if (moveable_check_out)
							{
								moveable_ptr = static_cast<const Moveable*>(moveable_check_out);
							}
							else
							{
								continue;
							}

						}
						

					}
				}

				//check data back in
				gCoordinator.CheckInComponent(ComponentTypeEnum::EVENT, entity_actable);

				entities_to_cleanup.push_back(entity_actable);
			}
		}


		for(Entity entity_to_clean : entities_to_cleanup)
		{
			gCoordinator.DestroyEntity(entity_to_clean);
		}
	}

}