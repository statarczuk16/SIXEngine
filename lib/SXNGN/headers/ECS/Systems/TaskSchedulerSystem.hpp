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

		bool AssignJobToWorker(Entity worker_id, Task_Worker_Component* worker, Entity task_id, Task_Component* task);

	private:


	};
}