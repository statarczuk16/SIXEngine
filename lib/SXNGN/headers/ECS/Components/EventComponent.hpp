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
		PARTY,
		FUNCTION,
		CHOICE
	};

	enum PartyEventType 
	{
		NONE,
		ROAD,
		ROAD_START,
		ROAD_BAD, //order is important group bad, good, together
		ROAD_BAD_BOOTS,
		ROAD_BAD_LOST,
		ROAD_BAD_ROBBER,
		ROAD_BAD_SICK,
		ROAD_GOOD,
		ROAD_GOOD_LOOT,
		ROAD_NEUTRAL,
		ROAD_END,
		RUINS,
		RUINS_START,
		RUINS_BAD,
		RUINS_BAD_ROBBER,
		RUINS_BAD_COLLAPSE,
		RUINS_GOOD,
		RUINS_GOOD_LOOT,
		RUINS_NEUTRAL,
		RUINS_END,
		SETTLEMENT,
		SETTLEMENT_START,
		SETTLEMENT_BAD,
		SETTLEMENT_BAD_PICKPOCKET,
		SETTLEMENT_BAD_ROBBER,
		SETTLEMENT_GOOD,
		SETTLEMENT_GOOD_LOOT,
		SETTLEMENT_GOOD_RUMOR,
		SETTLEMENT_NEUTRAL,
		SETTLEMENT_END,
		ANY,
		ANY_START,
		ANY_BAD,
		ANY_BAD_WEATHER,
		ANY_GOOD,
		ANY_STRANGER,
		ANY_NEUTRAL,
		ANY_END,
	};

	//convert enum to string for json purposes
	inline std::unordered_map<PartyEventType, std::string>& party_event_type_enum_to_string() {
		static std::unordered_map<PartyEventType, std::string> party_event_type_enum_to_string =
		{

		{PartyEventType::NONE,"NONE"},
		{PartyEventType::ROAD_START,"ROAD_START"},
		{PartyEventType::ROAD,"ROAD"},
		{PartyEventType::ROAD_BAD,"ROAD_BAD"}, //order is important group bad, good, together
		{PartyEventType::ROAD_BAD_BOOTS,"ROAD_BAD_BOOTS"},
		{PartyEventType::ROAD_BAD_LOST,"ROAD_BAD_LOST"},
		{PartyEventType::ROAD_BAD_ROBBER,"ROAD_BAD_ROBBER"},
		{PartyEventType::ROAD_BAD_SICK,"ROAD_BAD_SICK"},
		{PartyEventType::ROAD_GOOD,"ROAD_GOOD"},
		{PartyEventType::ROAD_GOOD_LOOT,"ROAD_GOOD_LOOT"},
		{PartyEventType::ROAD_NEUTRAL,"ROAD_NEUTRAL"},
		{PartyEventType::ROAD_END,"ROAD_END"},
		{PartyEventType::RUINS_START,"RUINS_START"},
		{PartyEventType::RUINS,"RUINS"},
		{PartyEventType::RUINS_BAD,"RUINS_BAD"},
		{PartyEventType::RUINS_BAD_ROBBER,"RUINS_BAD_ROBBER"},
		{PartyEventType::RUINS_BAD_COLLAPSE,"RUINS_BAD_COLLAPSE"},
		{PartyEventType::RUINS_GOOD,"RUINS_GOOD"},
		{PartyEventType::RUINS_GOOD_LOOT,"RUINS_GOOD_LOOT"},
		{PartyEventType::RUINS_NEUTRAL,"RUINS_NEUTRAL"},
		{PartyEventType::RUINS_END,"RUINS_END"},
		{PartyEventType::SETTLEMENT_START,"SETTLEMENT_START"},
		{PartyEventType::SETTLEMENT,"SETTLEMENT"},
		{PartyEventType::SETTLEMENT_BAD,"SETTLEMENT_BAD"},
		{PartyEventType::SETTLEMENT_BAD_PICKPOCKET,"SETTLEMENT_BAD_PICKPOCKET"},
		{PartyEventType::SETTLEMENT_BAD_ROBBER,"SETTLEMENT_BAD_ROBBER"},
		{PartyEventType::SETTLEMENT_GOOD,"SETTLEMENT_GOOD"},
		{PartyEventType::SETTLEMENT_GOOD_LOOT,"SETTLEMENT_GOOD_LOOT"},
		{PartyEventType::SETTLEMENT_GOOD_RUMOR,"SETTLEMENT_GOOD_RUMOR"},
		{PartyEventType::SETTLEMENT_NEUTRAL,"SETTLEMENT_NEUTRAL"},
		{PartyEventType::SETTLEMENT_END,"SETTLEMENT_END"},
		{PartyEventType::ANY_START,"ANY_START"},
		{PartyEventType::ANY,"ANY"},
		{PartyEventType::ANY_BAD,"ANY_BAD"},
		{PartyEventType::ANY_BAD_WEATHER,"ANY_BAD_WEATHER"},
		{PartyEventType::ANY_GOOD,"ANY_GOOD"},
		{PartyEventType::ANY_STRANGER,"ANY_STRANGER"},
		{PartyEventType::ANY_NEUTRAL,"ANY_NEUTRAL"},
		{PartyEventType::ANY_END,"ANY_END"}
		};
		return party_event_type_enum_to_string;
	}

	//convert string to enum for json purposes
	inline std::unordered_map<std::string, PartyEventType>& party_event_type_string_to_enum() {
		static std::unordered_map<std::string, PartyEventType> party_event_type_string_to_enum =
		{
		{"NONE", PartyEventType::NONE},
		{"ROAD_START",PartyEventType::ROAD_START},
		{"ROAD",PartyEventType::ROAD},
		{"ROAD_BAD",PartyEventType::ROAD_BAD}, //order is important group bad, good, together
		{"ROAD_BAD_BOOTS",PartyEventType::ROAD_BAD_BOOTS},
		{"ROAD_BAD_LOST",PartyEventType::ROAD_BAD_LOST},
		{"ROAD_BAD_ROBBER",PartyEventType::ROAD_BAD_ROBBER},
		{"ROAD_BAD_SICK",PartyEventType::ROAD_BAD_SICK},
		{"ROAD_GOOD",PartyEventType::ROAD_GOOD},
		{"ROAD_GOOD_LOOT",PartyEventType::ROAD_GOOD_LOOT},
		{"ROAD_NEUTRAL",PartyEventType::ROAD_NEUTRAL},
		{"ROAD_END",PartyEventType::ROAD_END},
		{"RUINS_START",PartyEventType::RUINS_START},
		{"RUINS",PartyEventType::RUINS},
		{"RUINS_BAD",PartyEventType::RUINS_BAD},
		{"RUINS_BAD_ROBBER",PartyEventType::RUINS_BAD_ROBBER},
		{"RUINS_BAD_COLLAPSE",PartyEventType::RUINS_BAD_COLLAPSE},
		{"RUINS_GOOD",PartyEventType::RUINS_GOOD},
		{"RUINS_GOOD_LOOT",PartyEventType::RUINS_GOOD_LOOT},
		{"RUINS_NEUTRAL",PartyEventType::RUINS_NEUTRAL},
		{"RUINS_END",PartyEventType::RUINS_END},
		{"SETTLEMENT_START",PartyEventType::SETTLEMENT_START},
		{"SETTLEMENT",PartyEventType::SETTLEMENT},
		{"SETTLEMENT_BAD",PartyEventType::SETTLEMENT_BAD},
		{"SETTLEMENT_BAD_PICKPOCKET",PartyEventType::SETTLEMENT_BAD_PICKPOCKET},
		{"SETTLEMENT_BAD_ROBBER",PartyEventType::SETTLEMENT_BAD_ROBBER},
		{"SETTLEMENT_GOOD",PartyEventType::SETTLEMENT_GOOD},
		{"SETTLEMENT_GOOD_LOOT",PartyEventType::SETTLEMENT_GOOD_LOOT},
		{"SETTLEMENT_GOOD_RUMOR",PartyEventType::SETTLEMENT_GOOD_RUMOR},
		{"SETTLEMENT_NEUTRAL",PartyEventType::SETTLEMENT_NEUTRAL},
		{"SETTLEMENT_END",PartyEventType::SETTLEMENT_END},
		{"ANY_START",PartyEventType::ANY_START},
		{"ANY",PartyEventType::ANY},
		{"ANY_BAD",PartyEventType::ANY_BAD},
		{"ANY_BAD_WEATHER",PartyEventType::ANY_BAD_WEATHER},
		{"ANY_GOOD",PartyEventType::ANY_GOOD},
		{"ANY_STRANGER",PartyEventType::ANY_STRANGER},
		{"ANY_NEUTRAL",PartyEventType::ANY_NEUTRAL},
		{"ANY_END",PartyEventType::ANY_END}
		};
		return party_event_type_string_to_enum;
	}


	enum class EventSeverity : Uint8
	{
		UNKNOWN = 0,
		MILD = 1,
		MEDIUM = 2,
		SPICY = 3,
		EXTREME = 4
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
		std::unordered_map<ItemType, double> items_gained;
		std::unordered_map<ItemType, double> items_lost;
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

	struct SXNGN_FunctionEvent
	{
		std::vector< std::function<void()> > callbacks;
	};

	//spawn choice window
	struct SXNGN_ChoiceEvent
	{
		bool pause; //pause the game if true
		std::string title; //title of choice
		std::string detail; //details
		std::vector <std::string> options_text; //text on the choice buttons per button
		std::vector <std::function<void()> > options_callbacks;//functions that the buttons do when clicked
		//std::vector <std::function<void()> > options_enables;//must return true or the button will be disabled
		std::vector <bool> option_enables; //must be true of button will be disabled (this has higher priority over option_enables)
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
		SXNGN_FunctionEvent func_event;
		SXNGN_ChoiceEvent choice_event;
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