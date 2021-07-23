#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>
#include <sole.hpp>
#include <stack>


namespace SXNGN::ECS::A {

	struct TaskPair
	{
		TaskPair(Entity job_entity, Uint32 job_priority)
		{
			priority = job_priority;
			entity = job_entity;
		}
		bool operator < (const TaskPair& str) const
		{
			return (priority < str.priority);
		}
		Uint32 priority = 0;
		Entity entity = -1;
	};

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
		std::map<TaskSkill, SkillPriority> skill_enable_;
		Uint32 current_job_priority_ = 0;
		Entity current_job_ = -1;
		std::map<SkillPriority, std::vector<TaskPair>> job_queue_;
		bool new_job = false; //flag when adding a new job
		bool job_transition_ = false; //flag when starting a new job
	};

	
}