
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

		Schedule_Jobs();


	}

	
	TaskPair Task_Scheduler_System::FindHighestPriorityJob(Task_Worker_Component*  worker, bool remove)
	{
		std::map<SkillPriority, std::vector<TaskPair>>::iterator it = worker->job_queue_.begin();
		//go from highest to lowest priority job queue
		while (it != worker->job_queue_.end())
		{
			if (it->second.empty())
			{
				continue;
			}
			//sort from lowest to highest priority
			std::sort(it->second.begin(), it->second.end());
			//return highest priority job
			TaskPair highest = it->second.at(it->second.size());
			if (remove)
			{
				it->second.pop_back();
			}
			return highest;
		}
	}

	void Task_Scheduler_System::Work_Workers()
	{
		auto gCoordinator = *SXNGN::Database::get_coordinator();
		std::vector<Entity> entities_to_cleanup;

		//Iterate through entities this system manipulates/converts
		auto worker_it = m_entities_of_interest.begin();
		//actable entities are jobs
		while (worker_it != m_entities_of_interest.end())
		{
			auto const& worker_id = *worker_it;
			worker_it++;

			ECS_Component* worker_ptr = nullptr;
			if(worker_ptr = gCoordinator.CheckOutComponent(worker_id, ComponentTypeEnum::TASK_WORKER));
			{
				//When no job, find the highest priority and start it
				Task_Worker_Component* worker = static_cast<Task_Worker_Component*>(worker_ptr);
				if (worker->current_job_ == -1)
				{
					//pop highest priority job from worker queue
					TaskPair tasked_job = FindHighestPriorityJob(worker, true);
					worker->current_job_ = tasked_job.entity;
					worker->current_job_priority_ = tasked_job.priority;
					worker->job_transition_ = true;
				}
				ECS_Component* moveable_ptr = nullptr;
				Moveable* moveable = nullptr;
				const ECS_Component* task_ptr = nullptr;
				const Task_Component* task = nullptr;
				if (task_ptr = gCoordinator.GetComponentReadOnly(worker_id, ComponentTypeEnum::TASK))
				{
					task = static_cast<const Task_Component*>(task_ptr);

				}
				if (moveable_ptr = gCoordinator.CheckOutComponent(worker_id, ComponentTypeEnum::MOVEABLE));
				{
					moveable = static_cast<Moveable*>(moveable_ptr);
				}
				//if transitioning jobs, 
				if (worker->job_transition_)
				{
					worker->job_transition_ = false;
					if (moveable)
					{
						moveable->Update_Destination(task->tasks_.at(0).location_);
						moveable->SolveDestination(moveable->navigation_type);
					}
				}

				if (moveable)
				{
					gCoordinator.CheckInComponent(ComponentTypeEnum::MOVEABLE, worker_id);
				}
				if (worker)
				{
					gCoordinator.CheckInComponent(ComponentTypeEnum::TASK, worker_id);
				}

				

				
			}
			gCoordinator.CheckInComponent(ComponentTypeEnum::TASK_WORKER, worker_id);
		}
	}

	void Task_Scheduler_System::Schedule_Jobs()
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
					SDL_LogDebug(1, "Tasks completed: Entity: %d : UUID %s", task_id, task_ptr->id_.base62().c_str());
					entities_to_cleanup.push_back(task_id);
					continue;
				}
				//else, schedule the job
				if (task_ptr && !task_ptr->scheduled_)
				{

					//go through all workers and find the best worker for the job
					auto it_int = m_entities_of_interest.begin();
					std::map<Entity, Sint32> worker_candidate_map_;
					std::unordered_set<Entity> best_worker;
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
										best_worker.insert(entity_int);
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


		for (Entity entity_to_clean : entities_to_cleanup)
		{
			gCoordinator.DestroyEntity(entity_to_clean);
		}
	}

	bool Task_Scheduler_System::AssignJobToWorker(Entity worker_id, Task_Worker_Component* worker, Entity task_id, Task_Component* task)
	{
		
		SDL_LogInfo(1, "Job %d assigned to worker %d", task_id, worker_id);
		
		auto task_skill_priority = worker->skill_enable_.at(task->tasks_.at(0).skill_required_);
		TaskPair new_task_pair = TaskPair(task_id, task->priority_);
		worker->job_queue_.at(task_skill_priority).push_back(new_task_pair);
		worker->new_job = true;
		task->reserved_units_.insert(worker_id);
		task->scheduled_ = true;
		return true;
	}
}

