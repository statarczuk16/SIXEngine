#pragma once

#include "ECS/Core/System.hpp"
#include <memory>

 
namespace SXNGN::ECS::A {

	class Event;
	class Camera_System : public System
	{
	public:
		void Init();

		void Update(float dt);

	private:


	};
}
