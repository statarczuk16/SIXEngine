#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>
#include <sole.hpp>


namespace SXNGN::ECS::A {



	/// <summary>
	/// Contains all data needed to describe a task for a task_worker to work on
	/// </summary>
	struct Task_Component : ECS_Component
	{
		Task_Component()
		{
			id_ = sole::uuid1();
			component_type = ComponentTypeEnum::TASK;
		}

		sole::uuid id_;//unique id
		std::string name_ = "Uninit";//task name
		std::vector<WorkChunk> tasks_; //all the work to do
		std::vector<WorkChunk> tasks_done_;//move tasks to here when completed
		std::unordered_set<Entity> reserved_units_;//workers scheduled
		std::unordered_set<Entity> required_units_;//if task designed for specific units
		bool interruptable_ = true;//is work interruptable
		Uint8 priority_ = -1; //priority of task
		bool scheduled_ = false;

	};
}