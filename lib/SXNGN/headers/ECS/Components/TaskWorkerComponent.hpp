#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>
#include <sole.hpp>
#include <stack>


namespace SXNGN::ECS::A {

	struct TaskPair
	{
		TaskPair()
		{

		}
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
		Entity entity = MAX_ENTITIES;
	};

	inline void to_json(json& j, const TaskPair& p) {
		j = json{
			{"priority", p.priority},
			{"entity", p.entity},
		};
	}

	inline void from_json(const json& j, TaskPair& p) {
		j.at("priority").get_to(p.priority);
		j.at("entity").get_to(p.entity);
	}

	/// <summary>
	/// Component used by an entity to work on tasks
	/// </summary>
	struct Task_Worker_Component : ECS_Component
	{
		Task_Worker_Component()
		{
			component_type = ComponentTypeEnum::TASK_WORKER;
			current_job_ = MAX_ENTITIES;
			current_job_priority_ = 0;
			new_job = false;
			job_transition_ = false;
		}

		
		std::map<TaskSkill, Sint32> skill_levels_;
		std::map<TaskSkill, SkillPriority> skill_enable_;
		Uint32 current_job_priority_ = 0;
		Entity current_job_ = MAX_ENTITIES;
		std::map<SkillPriority, std::vector<TaskPair>> job_queue_;
		bool new_job = false; //flag when adding a new job
		bool job_transition_ = false; //flag when starting a new job
	};

	inline void to_json(json& j, const Task_Worker_Component& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::TASK_WORKER]},
			
			{"skill_levels_", p.skill_levels_},
			{"skill_enable_", p.skill_enable_},
			{"current_job_priority_", p.current_job_priority_},
			{"current_job_", p.current_job_},
			{"job_queue_", p.job_queue_},
			{"new_job", p.new_job},
			{"job_transition_", p.job_transition_},
		};
	}

	inline void from_json(const json& j, Task_Worker_Component& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		
		j.at("skill_levels_").get_to(p.skill_levels_);
		j.at("skill_enable_").get_to(p.skill_enable_);
		j.at("current_job_priority_").get_to(p.current_job_priority_);
		j.at("current_job_").get_to(p.current_job_);
		j.at("job_queue_").get_to(p.job_queue_);
		j.at("new_job").get_to(p.new_job);
		j.at("job_transition_").get_to(p.job_transition_);
	}

	
}