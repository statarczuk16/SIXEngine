#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>


using ComponentTypeEnum = SXNGN::ECS::A::ComponentTypeEnum;

namespace SXNGN::ECS::A {


	enum class EventType : Uint8
	{
		SAVE,
		LOAD,
		STATE_CHANGE
	};

	typedef struct SXNGN_Common
	{
		EventType type;
		Uint32 timestamp;
		std::string filePath;
	} SXNGN_Common;

	typedef struct SXNGN_SaveEvent 
	{
		EventType type;
		Uint32 timestamp;
		std::string filePath;
	} SXNGN_SaveEvent;

	typedef struct SXNGN_LoadEvent
	{
		EventType type;
		Uint32 timestamp;
		std::string filePath;
	} SXNGN_LoadEvent;

	typedef struct SXNGN_StateChangeEvent
	{
		EventType type;
		Uint32 timestamp;
		std::string new_state;
		std::string old_state;
		bool overwrite_old;
		bool overwrite_new;
	} SXNGN_StateChangeEvent;
	
	typedef union SXNGN_Event
	{
		EventType type;
		SXNGN_SaveEvent save;
		SXNGN_LoadEvent load;
		SXNGN_StateChangeEvent state_change;
	} SXNGN_Event;

	struct Event_Component : ECS_Component
	{
		Event_Component()
		{
			component_type = ComponentTypeEnum::EVENT;
		}

	};

	


}