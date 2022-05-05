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
			component_type = ComponentTypeEnum::TASK;
		}
		std::string name_ = "Uninit";//task name
		std::vector<WorkChunk> tasks_; //all the work to do
		std::vector<WorkChunk> tasks_done_;//move tasks to here when completed
		std::unordered_set<Entity> reserved_units_;//workers scheduled
		std::unordered_set<Entity> required_units_;//if task designed for specific units
		bool interruptable_ = true;//is work interruptable
		Uint8 priority_ = -1; //priority of task
		bool scheduled_ = false;
		bool canceled_ = false;
		bool suspended_ = false;

	};

	//todo enum to string for collision_tag
	inline void to_json(json& j, const Task_Component& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::TASK]},
			
			{"name_", p.name_},
			{"tasks_", p.tasks_},
			{"tasks_done_", p.tasks_done_},
			{"reserved_units_", p.reserved_units_},
			{"required_units_", p.required_units_},
			{"interruptable_", p.interruptable_},
			{"priority_", p.priority_},
			{"scheduled_", p.scheduled_},
			{"canceled_", p.canceled_},
			{"suspended_", p.suspended_},
		};
	}

	inline void from_json(const json& j, Task_Component& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		j.at("name_").get_to(p.name_);
		j.at("tasks_").get_to(p.tasks_);
		j.at("tasks_done_").get_to(p.tasks_done_);
		j.at("reserved_units_").get_to(p.reserved_units_);
		j.at("required_units_").get_to(p.required_units_);
		j.at("interruptable_").get_to(p.interruptable_);
		j.at("priority_").get_to(p.priority_);
		j.at("scheduled_").get_to(p.scheduled_);
		j.at("canceled_").get_to(p.canceled_);
		j.at("suspended_").get_to(p.suspended_);
	}
}