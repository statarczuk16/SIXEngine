#pragma once

#include "ECS/Core/System.hpp"
#include <memory>
#include <SDL.h>
#include <ECS/Components/Moveable.hpp>


namespace SXNGN::ECS::A {
	using Moveable = A::Moveable;
	using Renderable = A::Renderable;
	using MoveableType = A::MoveableType;
	//using Collisionable = SXNGN::ECS::A::Collisionable;

	class Event_System : public System
	{
	public:
		virtual void Init();

		void Update(double dt);

		void Handle_Order_Event(Event_Component* ec);

		void Handle_Spawn_Event(Event_Component* ec);

		void Handle_Mouse_Event(Event_Component* ec, Entity entity);

		void Handle_Mouse_Wheel_Event(Event_Component* ec);

		void Handle_Save_Event(Event_Component* ec);

		void Handle_Load_Event(Event_Component* ec);

	private:


	};
}