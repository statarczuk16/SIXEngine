#pragma once
#include <cstdint>
#include <nlohmann/json.hpp>
#include <SDL.h>
#include <unordered_map>
#include <mutex>
namespace SXNGN::ECS::A {

	enum class ComponentTypeEnum : Uint8
{
	UNKNOWN,
	RENDERABLE,
	RENDERABLE_BATCH,
	PRE_RENDERABLE,
	PRE_RENDERABLE_BATCH,
	SPRITE_FACTORY,
	PRE_SPRITE_FACTORY,
	CAMERA_SINGLE,
	MOVEABLE,
	INPUT_CACHE,
	INPUT_TAGS,
	SELECTABLE,
	UI_SINGLE,
	COLLISION,
	TILE,
	JSON_ENTITY,
	EXTERN_ENTITY,
	EVENT,
	TASK,
	TASK_WORKER,
	GAME_STATE_START_HERE, //only game state enums after this point
	MAIN_MENU_STATE,
	MAIN_GAME_STATE,
	NEW_GAME_STATE,
	MAIN_SETTINGS_STATE,
	CORE_BG_GAME_STATE//don't forget to add to component_type_enum_to_string and reverse when adding a new enum
};



	//convert enum to string for json purposes
	inline std::unordered_map<ComponentTypeEnum, std::string>& component_type_enum_to_string() {
		static std::unordered_map<ComponentTypeEnum, std::string> component_type_enum_to_string =
		{
			{ComponentTypeEnum::UNKNOWN,"UNKNOWN"},
			{ComponentTypeEnum::RENDERABLE,"RENDERABLE"},
			{ComponentTypeEnum::RENDERABLE_BATCH,"RENDERABLE_BATCH"},
			{ComponentTypeEnum::PRE_RENDERABLE,"PRE_RENDERABLE"},
			{ComponentTypeEnum::PRE_RENDERABLE_BATCH,"PRE_RENDERABLE_BATCH"},
			{ComponentTypeEnum::SPRITE_FACTORY,"SPRITE_FACTORY"},
			{ComponentTypeEnum::PRE_SPRITE_FACTORY,"PRE_SPRITE_FACTORY"},
			{ComponentTypeEnum::CAMERA_SINGLE,"CAMERA_SINGLE"},
			{ComponentTypeEnum::MOVEABLE,"MOVEABLE"},
			{ComponentTypeEnum::INPUT_CACHE,"INPUT_CACHE"},
			{ComponentTypeEnum::INPUT_TAGS,"INPUT_TAGS"},
			{ComponentTypeEnum::COLLISION,"COLLISION"},
			{ComponentTypeEnum::TASK,"TASK"},
			{ComponentTypeEnum::TASK_WORKER,"TASK_WORKER"},
			{ComponentTypeEnum::TILE,"TILE"},
			{ComponentTypeEnum::SELECTABLE,"SELECTABLE"},
			{ComponentTypeEnum::JSON_ENTITY,"JSON_ENTITY"},
			{ComponentTypeEnum::EXTERN_ENTITY,"EXTERN_ENTITY"},
			{ComponentTypeEnum::MAIN_MENU_STATE,"MAIN_MENU_STATE"},
			{ComponentTypeEnum::CORE_BG_GAME_STATE,"CORE_BG_GAME_STATE"},
			{ComponentTypeEnum::MAIN_GAME_STATE,"MAIN_GAME_STATE"},
			{ComponentTypeEnum::MAIN_SETTINGS_STATE,"MAIN_SETTINGS_STATE"},
			{ComponentTypeEnum::NEW_GAME_STATE,"NEW_GAME_STATE"}

		};
		return component_type_enum_to_string;
	}

	//convert string to enum for JSON purposes
	inline std::unordered_map<std::string, ComponentTypeEnum>& component_type_string_to_enum() {
		static std::unordered_map<std::string, ComponentTypeEnum> component_type_string_to_enum =
		{
			{"UNKNOWN",ComponentTypeEnum::UNKNOWN},
			{"RENDERABLE",ComponentTypeEnum::RENDERABLE},
			{"RENDERABLE_BATCH",ComponentTypeEnum::RENDERABLE_BATCH},
			{"PRE_RENDERABLE",ComponentTypeEnum::PRE_RENDERABLE},
			{"PRE_RENDERABLE_BATCH",ComponentTypeEnum::PRE_RENDERABLE_BATCH},
			{"SPRITE_FACTORY",ComponentTypeEnum::SPRITE_FACTORY},
			{"PRE_SPRITE_FACTORY",ComponentTypeEnum::PRE_SPRITE_FACTORY},
			{"CAMERA_SINGLE",ComponentTypeEnum::CAMERA_SINGLE},
			{"MOVEABLE",ComponentTypeEnum::MOVEABLE},
			{"INPUT_CACHE",ComponentTypeEnum::INPUT_CACHE},
			{"INPUT_TAGS",ComponentTypeEnum::INPUT_TAGS},
			{"COLLISION",ComponentTypeEnum::COLLISION},
			{"TASK",ComponentTypeEnum::TASK},
			{"TASK_WORKER",ComponentTypeEnum::TASK_WORKER},
			{"TILE",ComponentTypeEnum::TILE},
			{"SELECTABLE",ComponentTypeEnum::SELECTABLE},
			{"JSON_ENTITY",ComponentTypeEnum::JSON_ENTITY},
			{"EXTERN_ENTITY",ComponentTypeEnum::EXTERN_ENTITY},
			{"MAIN_MENU_STATE",ComponentTypeEnum::MAIN_MENU_STATE},
			{"CORE_BG_GAME_STATE",ComponentTypeEnum::CORE_BG_GAME_STATE},
			{"MAIN_GAME_STATE",ComponentTypeEnum::MAIN_GAME_STATE},
			{"MAIN_SETTINGS_STATE",ComponentTypeEnum::MAIN_SETTINGS_STATE},
			{"NEW_GAME_STATE",ComponentTypeEnum::NEW_GAME_STATE}
		};
		return component_type_string_to_enum;
	}




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

		ComponentTypeEnum component_type;
	};

}