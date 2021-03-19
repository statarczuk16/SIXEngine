#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>


using ComponentTypeEnum = SXNGN::ECS::A::ComponentTypeEnum;

namespace SXNGN::ECS::A {


	enum class EventType : Uint8
	{
		UNKNOWN,
		SAVE,
		LOAD,
		STATE_CHANGE
	};

	struct SXNGN_Common
	{
		EventType type;
		Uint32 timestamp;
	};

	struct SXNGN_SaveEvent
	{
		std::string filePath;
	};

	struct SXNGN_LoadEvent
	{
		std::string filePath;
	};

	struct SXNGN_StateChangeEvent
	{

		std::forward_list<ComponentTypeEnum> new_states;
		std::forward_list<ComponentTypeEnum> states_to_remove;
	};
	
	struct SXNGN_Event
	{
		SXNGN_Common common;
		SXNGN_SaveEvent save;
		SXNGN_LoadEvent load;
		SXNGN_StateChangeEvent state_change;
	};

	struct Event_Component : ECS_Component
	{

		Event_Component()
		{
			component_type = ComponentTypeEnum::EVENT;
			e.common.type = EventType::UNKNOWN;
		}
		SXNGN_Event e;

	};

	


}