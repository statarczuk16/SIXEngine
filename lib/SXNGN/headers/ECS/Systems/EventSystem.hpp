#pragma once

#include "ECS/Core/System.hpp"
#include <memory>
#include <SDL.h>
#include <ECS/Components/Moveable.hpp>


namespace SXNGN::ECS::System {
	using Moveable = Components::Moveable;
	using Renderable = Components::Renderable;
	using MoveableType = Components::MoveableType;
	//using Collisionable = SXNGN::ECS::Components::Collisionable;

	class Event_System : public System
	{
	public:
		void Init();

		void Update(float dt);

		void HandleEvent();

	private:


	};
}