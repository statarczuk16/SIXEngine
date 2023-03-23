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

		//Adjust values for drop events. IE dont spawn BAD_BOOTS event if player already has no boots.
		void PruneEvents(Director* director_ptr, Party* party_ptr, bool at_settlement, bool at_ruins, bool party_moving);

		DropEntry<PartyEventType>* FindEventByType(Director* director_ptr, PartyEventType search_val);

		DropEntry<PartyEventType> GenerateEventTable();


	private:
		


	};
}