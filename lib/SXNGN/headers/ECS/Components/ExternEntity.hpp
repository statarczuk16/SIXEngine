#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>
#include <unordered_set>
#include <mutex>



/// <summary>
/// An entity ID and its corresponding components exported from the coordinator
/// Can be inserted back into the coordinator or saved to file
/// then loaded from file 
/// </summary>

namespace SXNGN::ECS::Components {


	struct ExternJSONEntity : ECS_Component {
		Entity entity_id_;
		std::vector<json> entity_components_;
		ExternJSONEntity(Entity entity_id, std::vector<json> entity_components)
		{
			component_type = ComponentTypeEnum::EXTERN_ENTITY;
			entity_id_ = entity_id;
			entity_components_ = entity_components;
		}

	};
}
