#pragma once

#include <ECS/Core/System.hpp>
#include <Database.h>
#include <memory>
#include <vector>


namespace SXNGN::ECS {

	/// <summary>
	/// Handles input from the player
	/// Divided into ~3 main functions with precedence
	/// 1. Any event that is detected to interact with the UI singleton component (kiss_sdl stuff) is handled here and removed the SDL_Event vector
	/// 2. WASD movement is handled here and applied to moveable components
	/// 3. Mouse events (that weren't already handled by 1.) are used to update the User_Input_State singleton. Click, double click, dragging windows, etc. are detected and launched as events. (presuabmly for the collision system to handle)
	/// This system is intended to be the sole consumer of SDL_Events
	/// The precedence matters here. Events that hit the kiss_sdl library GUI elements should not also hit a unit that is lying under that spot in the game world. Double click events etc. are meant to interact with the game world, not the UI.
	/// </summary>
	class Parallax_System : public System
	{
	public:
		virtual void Init();

		void Update(double dt);


	private:
		


	};
}