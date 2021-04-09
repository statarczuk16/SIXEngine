#pragma once

#include <bitset>
#include <cstdint>
#include <SDL.h>


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
const Entity MAX_ENTITIES = 5000;
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

#define METHOD_LISTENER(EventType, Listener) EventType, std::bind(&Listener, this, std::placeholders::_1)
#define FUNCTION_LISTENER(EventType, Listener) EventType, std::bind(&Listener, std::placeholders::_1)

// TODO: Make these easier to define and use (macro?)
// TODO: Add some kind of enforcement/automation that a SetParam type and a GetParam type match

namespace Events {

	namespace Window {
		const EventId QUIT = "Events::Window::QUIT"_hash;
		const EventId RESIZED = "Events::Window::RESIZED"_hash;
		const EventId INPUT = "Events::Window::INPUT"_hash;


		namespace Input {
			const ParamId INPUT = "Events::Window::Input::INPUT"_hash;
		}

		namespace Resized {
			const ParamId WIDTH = "Events::Window::Resized::WIDTH"_hash;
			const ParamId HEIGHT = "Events::Window::Resized::HEIGHT"_hash;
		}
	}
}

namespace SXNGN::ECS::A {

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
