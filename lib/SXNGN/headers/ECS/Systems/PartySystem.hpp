#pragma once

#include <ECS/Core/System.hpp>
#include <Database.h>
#include <memory>
#include <vector>


namespace SXNGN::ECS::A {

	class Party_System : public System
	{
	public:
		virtual void Init();

		void Update(float dt);


	private:
		


	};
}