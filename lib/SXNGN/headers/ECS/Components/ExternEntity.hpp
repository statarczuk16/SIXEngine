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

namespace SXNGN::ECS::A {


	struct ExternJSONEntity : ECS_Component {
		Entity entity_id_;
		sole::uuid uuid_;
		std::vector<json> entity_components_;
		ExternJSONEntity(Entity entity_id, std::vector<json> entity_components, sole::uuid uuid)
		{
			component_type = ComponentTypeEnum::JSON_ENTITY;
			entity_id_ = entity_id;
			entity_components_ = entity_components;
			uuid_ = uuid;
		}


		//default constructor for JSON
		ExternJSONEntity()
		{
			component_type = ComponentTypeEnum::JSON_ENTITY;
			entity_id_ = -1;
			uuid_ = BAD_UUID;
		}

	};

	inline void to_json(json& j, const ExternJSONEntity& p) {
		auto component_type_str = component_type_enum_to_string().at(p.component_type);
		j = json{ {"component_type",component_type_str}, {"entity_id_",p.entity_id_},{"uuid_",p.uuid_} ,{"entity_components_",p.entity_components_} };
	}

	inline void from_json(const json& j, ExternJSONEntity& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		j.at("entity_id_").get_to(p.entity_id_);
		j.at("entity_components_").get_to(p.entity_components_);
		j.at("uuid_").get_to(p.uuid_);

	}

	/// <summary>
	/// Used to hold an entity id and all its components outside of the coordinator
	/// </summary>
	struct ExternEntity : ECS_Component {
		Entity entity_id_;
		sole::uuid uuid_;
		std::vector<ECS_Component*> entity_components_;
		ExternEntity(Entity entity_id, sole::uuid uuid, std::vector<ECS_Component*> entity_components)
		{
			component_type = ComponentTypeEnum::EXTERN_ENTITY;
			entity_id_ = entity_id;
			entity_components_ = entity_components;
			uuid_ = uuid;
		}

		/**
		ExternEntity(Entity entity_id, std::vector<ECS_Component*> entity_components)
		{
			component_type = ComponentTypeEnum::EXTERN_ENTITY;
			entity_id_ = entity_id;
			entity_components_ = entity_components;
		}
		**/

		//default constructor for JSON
		ExternEntity()
		{
			component_type = ComponentTypeEnum::EXTERN_ENTITY;
			entity_id_ = -1;
			uuid_ = SXNGN::BAD_UUID;
		}

	};

}
