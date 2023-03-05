#pragma once

#include <ECS/Core/System.hpp>
#include <ECS/Components/Director.hpp>
#include <Database.h>
#include <memory>
#include <vector>


namespace SXNGN::ECS {

	class Director_System : public System
	{
	public:
		virtual void Init();

		void Update(double dt);

		DropEntry<PartyEventType> GenerateEventTable();


	private:
		


	};
}