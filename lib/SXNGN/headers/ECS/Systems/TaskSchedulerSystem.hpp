#pragma once

#include "ECS/Core/System.hpp"
#include <memory>
#include <SDL.h>
#include <ECS/Components/TaskComponent.hpp>
#include <ECS/Components/TaskWorkerComponent.hpp>

namespace SXNGN::ECS::A {

	class Task_Scheduler_System : public System
	{
	public:
		void Init();

		void Update(float dt);

		void Schedule_Jobs();

		void Work_Workers(float dt);

		bool AssignJobToWorker(Entity worker_id, Task_Worker_Component* worker, Entity task_id, Task_Component* task);

		TaskPair FindHighestPriorityJob(Task_Worker_Component* worker, bool remove);

		bool Worker_Can_Do_Work(Task_Worker_Component* worker, Location* worker_loc, Task_Component* task);

		void Worker_Do_Work(Task_Worker_Component * worker, Task_Component * task, double dt);

	private:


	};
}