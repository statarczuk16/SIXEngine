#pragma once

#include <ECS/Core/System.hpp>
#include <UI/UserInputUtils.hpp>
#include <ECS/Components/UserInput.hpp>
#include <Database.h>
#include <ECS/Components/Moveable.hpp>
#include <memory>
#include <vector>
using User_Input_Cache = SXNGN::ECS::A::User_Input_Cache;
using User_Input_Tags = SXNGN::ECS::A::User_Input_Tags;
using Moveable = SXNGN::ECS::A::Moveable;
using User_Input_State = SXNGN::ECS::A::User_Input_State;
using User_Input_Tags_Collection = SXNGN::ECS::A::User_Input_Tags_Collection;
using MoveableType = SXNGN::ECS::A::MoveableType;
using UserInputUtils = SXNGN::UserInputUtils;

namespace SXNGN::ECS::A {

	class User_Input_System : public System
	{
	public:
		void Init();

		void Update(float dt);

		void Update_Mouse_State(std::vector<SDL_Event> mouse_events, float dt);

		void Translate_User_Input_To_Movement(Moveable* moveable, Entity entity, std::vector<SDL_Event> keyboard_inputs, float dt);


	private:


	};
}