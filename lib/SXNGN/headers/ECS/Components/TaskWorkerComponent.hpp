#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>
#include <sole.hpp>


namespace SXNGN::ECS::A {



	/// <summary>
	/// Contains all data needed to describe a task for a task_worker to work on
	/// </summary>
	struct Task_Worker_Component : ECS_Component
	{
		Task_Worker_Component()
		{
			id_ = sole::uuid1();
			component_type = ComponentTypeEnum::TASK_WORKER;
		}

		sole::uuid id_;//unique id
		std::map<TaskSkill, Sint32> skill_levels_;
		std::map<TaskSkill, Sint32> skill_enable_;
		Sint32 current_job_priority_ = -1;
		Entity current_job_ = -1;
		std::vector<Entity> job_queue_;
		bool new_job = false; //flag when adding a new job
	};
}