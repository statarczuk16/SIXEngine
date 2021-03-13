#pragma once

#include "ECS/Core/Types.hpp"
#include <set>
#include <vector>


namespace SXNGN::ECS::System {

	class System
	{
	public:
		//List of entities that contain all components needed by this system. Maintained by SystemManager
		//Only entites that have ALL of the Components of this System's Signature are added to this list
		//An entity that loses a component in the Signature will be removed
		std::set<Entity> m_actable_entities;

		//List of entities whose data this system needs, but that it doesn't act on.
		//An entity that ANY of the components in this System's signature will be added to this list
		std::set<Entity> m_entities_of_interest;

		//A System's main loop will (probably) iterate over m_actable_entities every frame
		//But will only loop over m_accessible_entities if an actable_entity required data from
		//an accessable_entity

		//Eg. Sprite_Factory is a component that can turn a Pre_Renderable into a Renderable
		//Sprite_Factory_System takes in Pre_Renderables and uses the entity containing the corresponding
		//Sprite_Factory to turn it into a Renerbble



	protected:
		//Entities to delete after update()
		std::vector<Entity> entities_to_cleanup;

	};
}
