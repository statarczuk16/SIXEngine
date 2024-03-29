
#include<Collision.h>
#include <ECS/Core/Coordinator.hpp>
#include <Database.h>
#include <ECS/Systems/TaskSchedulerSystem.hpp>
#include <ECS/Components/EventComponent.hpp>
#include <ECS/Components/TaskWorkerComponent.hpp>
#include <ECS/Utilities/Map_Utils.hpp>


namespace SXNGN::ECS {

	void Task_Scheduler_System::Init()
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Task_Scheduler_System Init");
	}

	void Task_Scheduler_System::Update(float dt)
	{

		Schedule_Jobs();
		Work_Workers(dt);


	}

	
	TaskPair Task_Scheduler_System::FindHighestPriorityJob(Task_Worker_Component*  worker, bool remove)
	{
		std::map<SkillPriority, std::vector<TaskPair>>::iterator it = worker->job_queue_.begin();
		TaskPair highest = TaskPair(MAX_ENTITIES, 0);
		//go from highest to lowest priority job queue
		while (it != worker->job_queue_.end())
		{
			if (it->second.empty())
			{
				it++;
				continue;
			}
			//sort from lowest to highest priority
			std::sort(it->second.begin(), it->second.end());
			//return highest priority job
			highest = it->second.back();
			if (remove)
			{
				it->second.pop_back();
			}
			it++;
			return highest;
		}
		return highest;
	}

	void Task_Scheduler_System::Work_Workers(float dt)
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
			Task_Worker_Component* worker = nullptr;
			ECS_Component* worker_ptr = nullptr;
			worker_ptr = gCoordinator.CheckOutComponent(worker_id, ComponentTypeEnum::TASK_WORKER);
			
			if(worker_ptr)
			{
				//When no job, find the highest priority and start it
				worker = static_cast<Task_Worker_Component*>(worker_ptr);
				worker = static_cast<Task_Worker_Component*>(worker_ptr);
				if (worker->current_job_ == MAX_ENTITIES)
				{
					//pop highest priority job from worker queue
					TaskPair tasked_job = FindHighestPriorityJob(worker, true);
					if (tasked_job.entity == MAX_ENTITIES)
					{
						gCoordinator.CheckInComponent(ComponentTypeEnum::TASK_WORKER, worker_id);
						continue;
					}
					worker->current_job_ = tasked_job.entity;
					worker->current_job_priority_ = tasked_job.priority;
					worker->job_transition_ = true;
				}
				Entity job_id = worker->current_job_;
				ECS_Component* moveable_ptr = nullptr;
				Moveable* worker_moveable = nullptr;
				Location* worker_location = nullptr;
				ECS_Component* task_ptr = nullptr;
				Task_Component* task = nullptr;
				task_ptr = gCoordinator.CheckOutComponent(job_id, ComponentTypeEnum::TASK);
				if (!task_ptr)
				{
					gCoordinator.CheckInComponent(ComponentTypeEnum::TASK_WORKER, worker_id);
					continue;
				}
				task = static_cast<Task_Component*>(task_ptr);

				if (moveable_ptr = gCoordinator.CheckOutComponent(worker_id, ComponentTypeEnum::MOVEABLE))
				{
					worker_moveable = static_cast<Moveable*>(moveable_ptr);
				}
				if (ECS_Component* location_data = gCoordinator.CheckOutComponent(worker_id, ComponentTypeEnum::LOCATION))
				{
					worker_location = static_cast<Location*>(location_data);
				}
				//if transitioning jobs, 
				if (worker->job_transition_)
				{
					worker->job_transition_ = false;
					if (worker_moveable)
					{
						worker_moveable->Update_Destination(task->tasks_.at(0).location_);
						bool path_to_dest_exists = worker_moveable->SolveDestination(worker_location->m_pos_x_m_, worker_location->m_pos_y_m_, worker_moveable->navigation_type_);
						if (path_to_dest_exists == false)
						{
							SDL_LogDebug(1, "Worker %d Canceled Job %d : Can't Reach", worker_id, job_id);
							task->canceled_ = true;
							worker->current_job_ = MAX_ENTITIES;
						}
					}
				}
				else
				{
					if (worker_moveable->Check_At_Destination(worker_location->m_pos_x_m_, worker_location->m_pos_y_m_))
					{
						if (Worker_Can_Do_Work(worker, worker_location, task))
						{
							Worker_Do_Work(worker, task, dt);
						}
						else
						{
							SDL_LogDebug(1, "Worker %d Canceled Job %d  at %d, %d: Can't Do Work", worker_id, job_id, task->tasks_.at(0).location_.x / SXNGN::BASE_TILE_WIDTH, task->tasks_.at(0).location_.y / SXNGN::BASE_TILE_HEIGHT);

							task->canceled_ = true;
							worker->current_job_ = MAX_ENTITIES;
						}
					}
				}



				if (worker_location)
				{
					gCoordinator.CheckInComponent(ComponentTypeEnum::LOCATION, worker_id);
				}
				if (worker_moveable)
				{
					gCoordinator.CheckInComponent(ComponentTypeEnum::MOVEABLE, worker_id);
				}
				if (worker)
				{
					gCoordinator.CheckInComponent(ComponentTypeEnum::TASK_WORKER, worker_id);
				}
				if (task)
				{
					gCoordinator.CheckInComponent(ComponentTypeEnum::TASK, job_id);
				}
			}
			
		}
	}

	bool Task_Scheduler_System::Worker_Can_Do_Work(Task_Worker_Component* worker, Location* worker_loc, Task_Component* task)
	{
		if (task->tasks_.empty())
		{
			return false;
		}
		if (worker_loc)
		{
			auto distance = Map_Utils::GetDistance(NAVIGATION_TYPE::MANHATTAN, worker_loc->GetPixelCoordinate(), task->tasks_.at(0).location_);
			if (distance > AT_DESTINATION_THRESH)
			{
				return false;
			}
		}
		if (worker->skill_levels_.count(task->tasks_.at(0).skill_required_) == 0)
		{
			return false;
		}
		if (worker->skill_levels_[task->tasks_.at(0).skill_required_] < task->tasks_.at(0).skill_level_required_)
		{
			return false;
		}
		return true;

	}

	void Task_Scheduler_System::Worker_Do_Work(Task_Worker_Component* worker, Task_Component* task, double dt)
	{
		double skill_multiplier = 0.2 * worker->skill_levels_.at(task->tasks_.at(0).skill_required_);
		double work_complete = dt * skill_multiplier;
		int work_tiks = (int) round(work_complete * 1000);
		//TODO use more than first index
		task->tasks_.front().work_completed_ += work_tiks;
		if(task->tasks_.front().work_completed_ > task->tasks_.at(0).work_required_)
		{
			SDL_LogDebug(1, "Work completed on %s", task->name_);
			task->tasks_.erase(task->tasks_.begin());
			worker->current_job_ = MAX_ENTITIES;
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
					SDL_LogDebug(1, "Tasks completed: Entity: %d : %s", task_id, task_ptr->name_);
					entities_to_cleanup.push_back(task_id);
					
				}
				else if (task_ptr->canceled_)
				{
					SDL_LogDebug(1, "Task canceled: Entity: %d : %s", task_id, task_ptr->name_);
					entities_to_cleanup.push_back(task_id);
					
				}
				//else, schedule the job
				else if (task_ptr && !task_ptr->scheduled_)
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
							

							Task_Worker_Component* worker = nullptr;
							if (ECS_Component* worker_data = gCoordinator.CheckOutComponent(entity_int, ComponentTypeEnum::TASK_WORKER))
							{
								worker = static_cast<Task_Worker_Component*>(worker_data);
							}

							Location* worker_location = nullptr;
							if (ECS_Component* location_data = gCoordinator.CheckOutComponent(entity_int, ComponentTypeEnum::LOCATION))
							{
								worker_location = static_cast<Location*>(location_data);
							}

							Moveable* worker_moveable = nullptr;
							if (ECS_Component* move_data = gCoordinator.CheckOutComponent(entity_int, ComponentTypeEnum::MOVEABLE))
							{
								worker_moveable = static_cast<Moveable*>(move_data);
							}



							if (worker && worker_location && worker_moveable)
							{
								
								//if worker has the skill required to do the work the task current needs
								if (worker->skill_enable_[task_ptr->tasks_.at(0).skill_required_] > SkillPriority::NONE)
								{
									worker_candidate_map_[entity_int] = -1;//is a candidate

									//and is the closest to the job
									Coordinate start = worker_location->GetPixelCoordinate();
									Coordinate end = task_ptr->tasks_.at(0).location_;

									auto distance_cost = Map_Utils::GetDistance(NAVIGATION_TYPE::MANHATTAN, start, end);
									if (distance_cost < lowest_cost)
									{
										lowest_cost = distance_cost;
										best_worker.clear();
										best_worker.insert(entity_int);
									}
									worker_candidate_map_[entity_int] = distance_cost;//for debug
								}
								
								
							}

							if (worker)
							{
								gCoordinator.CheckInComponent(ComponentTypeEnum::TASK_WORKER, entity_int);
							}
							if (worker_location)
							{
								gCoordinator.CheckInComponent(ComponentTypeEnum::LOCATION, entity_int);
							}
							if (worker_moveable)
							{
								gCoordinator.CheckInComponent(ComponentTypeEnum::MOVEABLE, entity_int);
							}
							
							
							
							
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
		if (worker->job_queue_.count(task_skill_priority))
		{
			worker->job_queue_.at(task_skill_priority).push_back(new_task_pair);
		}
		else
		{
			std::vector<TaskPair> new_job_vector = { new_task_pair };
			worker->job_queue_[task_skill_priority] = new_job_vector;
		}
		
		worker->new_job = true;
		task->reserved_units_.insert(worker_id);
		task->scheduled_ = true;
		return true;
	}
}

