#pragma once
#include <cstdint>
#include <nlohmann/json.hpp>
namespace SXNGN::ECS::Components {

	enum class ComponentTypeEnum : std::uint8_t
{
	UNKNOWN,
	RENDERABLE,
	RENDERABLE_BATCH,
	PRE_RENDERABLE,
	PRE_RENDERABLE_BATCH,
	SPRITE_FACTORY,
	PRE_SPRITE_FACTORY,
	CAMERA,
	MOVEABLE,
	INPUT_CACHE,
	INPUT_TAGS,
	COLLISION,
	TILE
};


	struct ECS_Component
	{
	ECS_Component()
	{
		component_type = ComponentTypeEnum::UNKNOWN;
	}

	ComponentTypeEnum get_component_type()
	{
		return component_type;
	}

	protected:
		ComponentTypeEnum component_type;
	};

}