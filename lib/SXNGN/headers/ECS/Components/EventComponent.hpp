#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>
#include <UI/UserInputUtils.hpp>


using ComponentTypeEnum = SXNGN::ECS::ComponentTypeEnum;

namespace SXNGN::ECS {


	enum class EventType : Uint8
	{
		UNKNOWN,
		SAVE,
		LOAD,
		STATE_CHANGE,
		EXIT,
		MOUSE,
		MOUSE_WHEEL,
		SELECTION,
		ORDER,
		SPAWN,
		PARTY
	};

	enum PartyEventType 
	{
		BAD, //order is important group bad, good, together
		BAD_BOOTS,
		BAD_LOST,
		BAD_WEATHER,
		BAD_ROBBER,
		BAD_SICK,
		GOOD,
		NEUTRAL,
		NONE
	};

	//convert enum to string for json purposes
	inline std::unordered_map<PartyEventType, std::string>& party_event_type_enum_to_string() {
		static std::unordered_map<PartyEventType, std::string> party_event_type_enum_to_string =
		{
			{PartyEventType::BAD,"BAD"},
			{PartyEventType::BAD_BOOTS,"BAD_BOOTS"},
			{PartyEventType::BAD_LOST,"BAD_LOST"},
			{PartyEventType::BAD_WEATHER,"BAD_WEATHER"},
			{PartyEventType::BAD_ROBBER,"BAD_ROBBER"},
			{PartyEventType::BAD_SICK,"BAD_SICK"},
			{PartyEventType::GOOD,"GOOD"},
			{PartyEventType::NEUTRAL,"NEUTRAL"},
			{PartyEventType::NONE,"NONE"}
		};
		return party_event_type_enum_to_string;
	}

	//convert string to enum for json purposes
	inline std::unordered_map<std::string, PartyEventType>& party_event_type_string_to_enum() {
		static std::unordered_map<std::string, PartyEventType> party_event_type_string_to_enum =
		{
			{"BAD",PartyEventType::BAD},
			{"BAD_BOOTS",PartyEventType::BAD_BOOTS},
			{"BAD_LOST",PartyEventType::BAD_LOST},
			{"BAD_WEATHER",PartyEventType::BAD_WEATHER},
			{"BAD_ROBBER",PartyEventType::BAD_ROBBER},
			{"BAD_SICK",PartyEventType::BAD_SICK},
			{"GOOD",PartyEventType::GOOD},
			{"NEUTRAL",PartyEventType::NEUTRAL},
			{"NONE",PartyEventType::NONE}
		};
		return party_event_type_string_to_enum;
	}


	enum class EventSeverity : Uint8
	{
		UNKNOWN,
		MILD,
		MEDIUM,
		SPICY,
		EXTREME
	};

	struct SXNGN_Common
	{
		EventType type;
		Uint32 timestamp;
	};

	struct SXNGN_Party
	{
		sole::uuid party_id;
		std::string event_name;
		PartyEventType party_event_type;
		EventSeverity severity;
	};

	struct SXNGN_SaveEvent
	{
		std::string filePath;
	};

	struct SXNGN_Selection
	{
		std::vector<Entity> clicked_entities;
		std::vector<Entity> boxed_entities;
		std::vector<Entity> double_click_entities;
		bool additive = false; //new selection or adding to current selection (left click vs ctrl + left click etc)
		bool subtractive = false;//alt click
		bool enqueue = false;//shift click
	};

	struct SXNGN_Order
	{
		std::vector<Entity> clicked_entities;
		std::vector<Entity> boxed_entities;
		std::vector<Entity> double_click_entities;
		bool additive = false; //new selection or adding to current selection (left click vs ctrl + left click etc)
		bool subtractive = false;//alt click
		bool enqueue = false;//shift click
		OrderType order_type = OrderType::UNKNOWN;
	};

	struct SXNGN_LoadEvent
	{
		std::string filePath;
	};

	struct SXNGN_StateChangeEvent
	{
		bool hard_remove = false; //if true, purge entities in the state to remove.
		std::forward_list<ComponentTypeEnum> new_states;
		std::forward_list<ComponentTypeEnum> states_to_remove;
	};

	enum class MouseEventType : Uint8
	{
		CLICK,
		BOX
	};

	struct SXNGN_MouseEvent
	{
		MouseEventType type;
		Click click;
		bool shift_click;
		bool alt_click;
		bool ctrl_click;
	};

	struct SXNGN_MouseWheelEvent
	{
		int x_ = 0;
		int y_ = 0;
	};

	struct SXNGN_SpawnEvent
	{
		int x_ = 0;
		int y_ = 0;
		bool spawn_1_despawn_0;
	};
	
	struct SXNGN_Event
	{
		SXNGN_Common common;
		SXNGN_SaveEvent save;
		SXNGN_LoadEvent load;
		SXNGN_StateChangeEvent state_change;
		SXNGN_MouseEvent mouse_event;
		SXNGN_MouseWheelEvent mouse_wheel_event;
		SXNGN_Selection select_event;
		SXNGN_Order order_event;
		SXNGN_SpawnEvent spawn_event;
		SXNGN_Party party_event;
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