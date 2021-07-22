#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>


namespace SXNGN::ECS::A {


	/// <summary>
	/// Contains data displayed when selected
	/// </summary>
	struct Selectable : ECS_Component
	{
		Selectable()
		{
			component_type = ComponentTypeEnum::SELECTABLE;
		}
		std::string selection_text_ = "";
	};
}