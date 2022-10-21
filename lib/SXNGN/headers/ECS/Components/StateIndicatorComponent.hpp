#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>


namespace SXNGN::ECS::A {


	/// <summary>
	/// These Components have no purpose other than to indicate in what state an entity should be processed
	/// IE, a System with MainMenuStateComponent in its registry will only see entities with a MainMenuStateComponent
	/// 
	/// To change state from MainMenu to MainGame, remove the MainMenuStateComponent from all of the systems' registries and instead add MainGameStateComponent
	/// </summary>

	struct MainMenuStateComponent : ECS_Component
	{
		MainMenuStateComponent()
		{
			component_type = ComponentTypeEnum::MAIN_MENU_STATE;
		}
	};

	struct MainGameStateComponent : ECS_Component
	{
		MainGameStateComponent()
		{
			component_type = ComponentTypeEnum::MAIN_GAME_STATE;
		}
	};

	struct CoreBackGroundGameStateComponent : ECS_Component
	{
		CoreBackGroundGameStateComponent()
		{
			component_type = ComponentTypeEnum::CORE_BG_GAME_STATE;
		}
	};

	struct OverworldGameStateComponent : ECS_Component
	{
		OverworldGameStateComponent()
		{
			component_type = ComponentTypeEnum::OVERWORLD_STATE;
		}
	};

	struct TacticalGameStateComponent : ECS_Component
	{
		TacticalGameStateComponent()
		{
			component_type = ComponentTypeEnum::TACTICAL_STATE;
		}
	};

	static ECS_Component* Create_Gamestate_Component_from_Enum(ComponentTypeEnum game_state)
	{
		switch (game_state)
		{
		case ComponentTypeEnum::MAIN_GAME_STATE: return new MainGameStateComponent();
		case ComponentTypeEnum::MAIN_MENU_STATE: return new MainMenuStateComponent();
		case ComponentTypeEnum::CORE_BG_GAME_STATE: return new CoreBackGroundGameStateComponent();
		case ComponentTypeEnum::TACTICAL_STATE: return new TacticalGameStateComponent();
		case ComponentTypeEnum::OVERWORLD_STATE: return new OverworldGameStateComponent();
		default: 
		{
			printf("Error:: StateIndicatorComponent::Create_Gamestate_Component_from_Enum no game state named: %s", component_type_enum_to_string().at(game_state).c_str());
			abort();
		}
		}
	}


}