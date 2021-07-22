
#include<Collision.h>
#include <ECS/Core/Coordinator.hpp>
#include <Database.h>
#include <ECS/Systems/TaskSchedulerSystem.hpp>
#include <ECS/Components/EventComponent.hpp>
#include <ECS/Components/TaskWorkerComponent.hpp>
#include <ECS/Utilities/Map_Utils.hpp>


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
		//actable entities are jobs
		while (it_act != m_actable_entities.end())
		{
			auto const& task_id = *it_act;
			it_act++;

			//get the task data
			auto check_out_event = gCoordinator.CheckOutComponent(task_id, ComponentTypeEnum::TASK);
			if (check_out_event)
			{

				Task_Component* task_ptr = static_cast<Task_Component*>(check_out_event);
				//if the task has already had all its work completed, its done
				if (task_ptr->tasks_.empty())
				{
					SDL_LogDebug(1,"Task completed: Entity: %d : UUID %s",task_id, task_ptr->id_.base62().c_str());
					entities_to_cleanup.push_back(task_id);
					continue;
				}
				//else, schedule the job
				if (task_ptr && !task_ptr->scheduled_)
				{
					
					//go through all workers and find the best worker for the job
					auto it_int = m_entities_of_interest.begin();
					std::map<Entity, Sint32> worker_candidate_map_;
					std::vector<Entity> best_worker;
					Uint32 lowest_cost = 2 ^ 32 - 1;
					//if no required units, find best person for job
					if (task_ptr->required_units_.empty())
					{
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
								//and is the closest to the job
								auto moveable_check_out = gCoordinator.GetComponentReadOnly(entity_int, ComponentTypeEnum::MOVEABLE);
								const Moveable* moveable_ptr;
								if (moveable_check_out)
								{
									moveable_ptr = static_cast<const Moveable*>(moveable_check_out);
									Location start;
									start.x = (int)(round(moveable_ptr->m_pos_x_m));
									start.y = (int)(round(moveable_ptr->m_pos_y_m));

									Location end = task_ptr->tasks_.at(0).location_;

									auto distance_cost = Map_Utils::GetDistance(NAVIGATION_TYPE::MANHATTAN, start, end);
									if (distance_cost < lowest_cost)
									{
										lowest_cost = distance_cost;
										best_worker.clear();
										best_worker.push_back(entity_int);
									}
									worker_candidate_map_[entity_int] = distance_cost;//for debug

								}
								else
								{
									//... how to determine cost for things that don't move?
									continue;
								}
							}
							gCoordinator.CheckInComponent(ComponentTypeEnum::TASK_WORKER, entity_int);
						}
					}
					//else find the required units and assign them
					else
					{
						best_worker = task_ptr->required_units_;
					}
					//iterated through all workers. Who's the best?
					if (best_worker.empty())
					{
						//no one suitable for job
					}
					else
					{
						//assign best workers to the job
						for (Entity req_worker : best_worker)
						{
							auto check_out_worker = gCoordinator.CheckOutComponent(req_worker, ComponentTypeEnum::TASK_WORKER);
							Task_Worker_Component* worker_ptr = static_cast<Task_Worker_Component*>(check_out_worker);
							if (worker_ptr)
							{
								AssignJobToWorker(req_worker, worker_ptr, task_id, task_ptr);
								gCoordinator.CheckInComponent(ComponentTypeEnum::TASK_WORKER, req_worker);
							}
						}
					}
				}

				//check data back in
				gCoordinator.CheckInComponent(ComponentTypeEnum::TASK, task_id);

				entities_to_cleanup.push_back(task_id);
			}
		}


		for(Entity entity_to_clean : entities_to_cleanup)
		{
			gCoordinator.DestroyEntity(entity_to_clean);
		}
	}

	bool Task_Scheduler_System::AssignJobToWorker(Entity worker_id, Task_Worker_Component* worker, Entity task_id, Task_Component* task)
	{
		
		SDL_LogInfo(1, "Job %d assigned to worker %d", task_id, worker_id);
		worker->job_queue_.push_back(task_id);
		worker->new_job = true;
		task->required_units_.push_back(worker_id);
		task->scheduled_ = true;
		return true;
	}
}

