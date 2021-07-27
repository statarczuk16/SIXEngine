#pragma once

#include <bitset>
#include <cstdint>
#include <SDL.h>
#include <memory>





// Source: https://gist.github.com/Lee-R/3839813
constexpr std::uint32_t fnv1a_32(char const* s, std::size_t count)
{
	return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u; // NOLINT (hicpp-signed-bitwise)
}

constexpr std::uint32_t operator "" _hash(char const* s, std::size_t count)
{
	return fnv1a_32(s, count);
}


// ECS
using Entity = std::uint32_t;
const Entity MAX_ENTITIES = 20000;
using ComponentTypeHash = std::uint8_t;
const ComponentTypeHash MAX_COMPONENTS = 32;
using Signature = std::bitset<MAX_COMPONENTS>;


// Input
enum class InputButtons
{
	W,
	A,
	S,
	D,
	Q,
	E
};


// Events
using EventId = std::uint32_t;
using ParamId = std::uint32_t;


namespace SXNGN::ECS::A {

	class Event_Component;
	
	struct Location {
		Sint32 x = -1;
		Sint32 y = -1;

		Location()
		{
			x = -1;
			y = -1;
		}

		Location(Sint32 x_in, Sint32 y_in)
		{
			x = x_in;
			y = y_in;
		}
		
	};

	enum class OrderType : Uint8
	{
		UNKNOWN,
		MOVE
	};

	enum class TaskSkill : Uint8 {
		UNKNOWN,
		MINING,
		BUILDING,
		WALKING
	};

	enum class SkillPriority : Uint8 {
		NONE,
		LOW,
		MEDIUM,
		HIGH,
		CRITICAL
	};

	struct WorkChunk {
		TaskSkill skill_required_;//skill required at each location
		Uint16 work_required_;//how much work needed to be done at each place
		Uint16 work_completed_;//how much work already done at each place
		Location location_;//places where work is to be done
		Uint16 skill_level_required_;//skill level required at each location
		//std::function<void()> result_function_;//functions called when task completed
		std::shared_ptr<SXNGN::ECS::A::Event_Component> result_event_; //events that fire when task completed
	};

	enum class NAVIGATION_TYPE : Uint8 {
		UNKNOWN,
		A_STAR,
		MANHATTAN
	};

	enum class MOUSE_BUTTON : Uint8 {
		LEFT,
		RIGHT,
		CENTER,
		UNKNOWN
	};

	struct MouseCoord {
		bool down = false;
		Sint32 x = 0;
		Sint32 y = 0;
		Uint32 timestamp_down = 0;
		Uint32 timestamp_up = 0;
	};

	struct MouseState {
		MouseCoord left_button;
		MouseCoord right_button;
		MouseCoord middle_button;
	};

	struct Click {
		MOUSE_BUTTON button = MOUSE_BUTTON::UNKNOWN;
		Uint32 time_stamp = 0;
		Sint32 x = 0;
		Sint32 y = 0;
		bool double_click = false;
	};

	enum class UIType {
		UNKNOWN,
		WINDOW,
		BUTTON,
		SELECT_BUTTON,
		VSCROLLBAR,
		HSCROLLBAR,
		PROGRESSBAR,
		ENTRY,
		LABEL,
		TEXTBOX,
		COMBOBOX
	};

	enum class UILayer { //order matters - layer with precedence for mouse clicks must be at the top/closest to 0
		UNKNOWN,
		BOTTOM,
		MID,
		TOP
	};

}
