#pragma once

namespace SXNGN::ECS::Components {

enum class ComponentTypeEnum : std::uint8_t
{
	UNKNOWN,
	RENDERABLE,
	PRE_RENDERABLE,
	SPRITE_FACTORY,
	PRE_SPRITE_FACTORY
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