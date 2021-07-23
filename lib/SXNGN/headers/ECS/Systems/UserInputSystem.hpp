#pragma once

#include <ECS/Core/System.hpp>
#include <UI/UserInputUtils.hpp>
#include <ECS/Components/UserInput.hpp>
#include <Database.h>
#include <ECS/Components/Moveable.hpp>
#include <ECS/Components/UI.hpp>
#include <memory>
#include <vector>


namespace SXNGN::ECS::A {

	/// <summary>
	/// Handles input from the player
	/// Divided into ~3 main functions with precedence
	/// 1. Any event that is detected to interact with the UI singleton component (kiss_sdl stuff) is handled here and removed the SDL_Event vector
	/// 2. WASD movement is handled here and applied to moveable components
	/// 3. Mouse events (that weren't already handled by 1.) are used to update the User_Input_State singleton. Click, double click, dragging windows, etc. are detected and launched as events. (presuabmly for the collision system to handle)
	/// This system is intended to be the sole consumer of SDL_Events
	/// The precedence matters here. Events that hit the kiss_sdl library GUI elements should not also hit a unit that is lying under that spot in the game world. Double click events etc. are meant to interact with the game world, not the UI.
	/// </summary>
	class User_Input_System : public System
	{
	public:
		void Init();

		void Handle_GUI_Input(std::vector<SDL_Event>& events);

		void GUI_Handle_Events(std::vector<SDL_Event> &events, std::map<UILayer, std::vector<std::shared_ptr<UIContainerComponent>>> layer_to_ui_elements);

		bool GUI_Handle_Event(std::shared_ptr<Coordinator> coordinator, SDL_Event* e, std::shared_ptr<UIContainerComponent> component_in_layer);

		bool Hotkey_Handle_Events(std::shared_ptr<Coordinator> coordinator, std::vector<SDL_Event>& events);

		void Update(float dt);

		void Update_Mouse_State(std::vector<SDL_Event> mouse_events, float dt);

		void Translate_User_Input_To_Movement(Moveable* moveable, Entity entity, std::vector<SDL_Event> keyboard_inputs, float dt);

		void Translate_Waypoints_To_Movement(Moveable* moveable);


	private:


	};
}