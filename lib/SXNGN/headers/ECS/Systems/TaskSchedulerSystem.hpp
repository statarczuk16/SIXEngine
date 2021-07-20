#pragma once

#include "ECS/Core/System.hpp"
#include <memory>
#include <SDL.h>


namespace SXNGN::ECS::A {

	class Task_Scheduler_System : public System
	{
	public:
		void Init();

		void Update(float dt);

	private:


	};
}