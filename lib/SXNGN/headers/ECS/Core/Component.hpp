#pragma once
#include <cstdint>
#include <nlohmann/json.hpp>
#include <SDL.h>
#include <unordered_map>
#include <mutex>
#include <nlohmann/json.hpp>
using nlohmann::json;

namespace SXNGN::ECS {

	enum class ComponentTypeEnum : Uint8
{
	UNKNOWN,
	LOCATION,
	DIRECTOR,
	RENDERABLE,
	RENDERABLE_BATCH,
	PRE_RENDERABLE,
	PRE_RENDERABLE_BATCH,
	SPRITE_FACTORY,
	PRE_SPRITE_FACTORY,
	CAMERA_SINGLE,
	DATABASE_SINGLE,
	MOVEABLE,
	INPUT_CACHE,
	INPUT_TAGS,
	SELECTABLE,
	BATTLE,
	WORLD_MAP,
	WORLD_LOCATION,
	UI_SINGLE,
	COLLISION,
	TILE,
	PARALLAX,
	UUID,
	JSON_ENTITY,
	EXTERN_ENTITY,
	EVENT,
	TASK,
	MARKET,
	TASK_WORKER,
	PARTY,
	GAME_STATE_START_HERE, //!!!! only game state enums after this point !!!!
	MAIN_MENU_STATE,
	MAIN_GAME_STATE,
	NEW_GAME_STATE,
	MAIN_SETTINGS_STATE,
	TACTICAL_STATE,
	OVERWORLD_STATE,
	BATTLE_STATE,
	CORE_BG_GAME_STATE,//don't forget to add to component_type_enum_to_string and reverse when adding a new enum
	NUM_COMPONENT_TYPES
};



	//convert enum to string for json purposes
	inline std::unordered_map<ComponentTypeEnum, std::string>& component_type_enum_to_string() {
		static std::unordered_map<ComponentTypeEnum, std::string> component_type_enum_to_string =
		{
			{ComponentTypeEnum::UNKNOWN,"UNKNOWN"},
			{ComponentTypeEnum::LOCATION,"LOCATION"},
			{ComponentTypeEnum::DIRECTOR,"DIRECTOR"},
			{ComponentTypeEnum::EVENT,"EVENT"},
			{ComponentTypeEnum::PARTY,"PARTY"},
			{ComponentTypeEnum::RENDERABLE,"RENDERABLE"},
			{ComponentTypeEnum::RENDERABLE_BATCH,"RENDERABLE_BATCH"},
			{ComponentTypeEnum::PRE_RENDERABLE,"PRE_RENDERABLE"},
			{ComponentTypeEnum::PRE_RENDERABLE_BATCH,"PRE_RENDERABLE_BATCH"},
			{ComponentTypeEnum::SPRITE_FACTORY,"SPRITE_FACTORY"},
			{ComponentTypeEnum::PRE_SPRITE_FACTORY,"PRE_SPRITE_FACTORY"},
			{ComponentTypeEnum::CAMERA_SINGLE,"CAMERA_SINGLE"},
			{ComponentTypeEnum::DATABASE_SINGLE,"DATABASE_SINGLE"},
			{ComponentTypeEnum::MOVEABLE,"MOVEABLE"},
			{ComponentTypeEnum::INPUT_CACHE,"INPUT_CACHE"},
			{ComponentTypeEnum::INPUT_TAGS,"INPUT_TAGS"},
			{ComponentTypeEnum::COLLISION,"COLLISION"},
			{ComponentTypeEnum::TASK,"TASK"},
			{ComponentTypeEnum::BATTLE,"BATTLE"},
			{ComponentTypeEnum::TASK_WORKER,"TASK_WORKER"},
			{ComponentTypeEnum::TILE,"TILE"},
			{ComponentTypeEnum::WORLD_MAP,"WORLD_MAP"},
			{ComponentTypeEnum::WORLD_LOCATION,"WORLD_LOCATION"},
			{ComponentTypeEnum::PARALLAX,"PARALLAX"},
			{ComponentTypeEnum::UUID,"UUID"},
			{ComponentTypeEnum::UI_SINGLE,"UI_SINGLE"},
			{ComponentTypeEnum::SELECTABLE,"SELECTABLE"},
			{ComponentTypeEnum::JSON_ENTITY,"JSON_ENTITY"},
			{ComponentTypeEnum::EXTERN_ENTITY,"EXTERN_ENTITY"},
			{ComponentTypeEnum::MAIN_MENU_STATE,"MAIN_MENU_STATE"},
			{ComponentTypeEnum::MARKET,"MARKET"},
			{ComponentTypeEnum::CORE_BG_GAME_STATE,"CORE_BG_GAME_STATE"},
			{ComponentTypeEnum::MAIN_GAME_STATE,"MAIN_GAME_STATE"},
			{ComponentTypeEnum::MAIN_SETTINGS_STATE,"MAIN_SETTINGS_STATE"},
			{ComponentTypeEnum::TACTICAL_STATE,"TACTICAL_STATE"},
			{ComponentTypeEnum::BATTLE_STATE,"BATTLE_STATE"},
			{ComponentTypeEnum::OVERWORLD_STATE,"OVERWORLD_STATE"},
			{ComponentTypeEnum::NEW_GAME_STATE,"NEW_GAME_STATE"},
			{ComponentTypeEnum::GAME_STATE_START_HERE,"GAME_STATE_START_HERE"}

		};
		return component_type_enum_to_string;
	}

	//convert string to enum for JSON purposes
	inline std::unordered_map<std::string, ComponentTypeEnum>& component_type_string_to_enum() {
		static std::unordered_map<std::string, ComponentTypeEnum> component_type_string_to_enum =
		{
			{"UNKNOWN",ComponentTypeEnum::UNKNOWN},
			{"DIRECTOR",ComponentTypeEnum::DIRECTOR},
			{"EVENT",ComponentTypeEnum::EVENT},
			{"PARTY",ComponentTypeEnum::PARTY},
			{"LOCATION",ComponentTypeEnum::LOCATION},
			{"RENDERABLE",ComponentTypeEnum::RENDERABLE},
			{"RENDERABLE_BATCH",ComponentTypeEnum::RENDERABLE_BATCH},
			{"PRE_RENDERABLE",ComponentTypeEnum::PRE_RENDERABLE},
			{"PRE_RENDERABLE_BATCH",ComponentTypeEnum::PRE_RENDERABLE_BATCH},
			{"SPRITE_FACTORY",ComponentTypeEnum::SPRITE_FACTORY},
			{"PRE_SPRITE_FACTORY",ComponentTypeEnum::PRE_SPRITE_FACTORY},
			{"CAMERA_SINGLE",ComponentTypeEnum::CAMERA_SINGLE},
			{"DATABASE_SINGLE",ComponentTypeEnum::DATABASE_SINGLE},
			{"MOVEABLE",ComponentTypeEnum::MOVEABLE},
			{"BATTLE",ComponentTypeEnum::BATTLE},
			{"INPUT_CACHE",ComponentTypeEnum::INPUT_CACHE},
			{"INPUT_TAGS",ComponentTypeEnum::INPUT_TAGS},
			{"COLLISION",ComponentTypeEnum::COLLISION},
			{"TASK",ComponentTypeEnum::TASK},
			{"TASK_WORKER",ComponentTypeEnum::TASK_WORKER},
			{"TILE",ComponentTypeEnum::TILE},
			{"PARALLAX",ComponentTypeEnum::PARALLAX},
			{"UI_SINGLE",ComponentTypeEnum::UI_SINGLE},
			{"UUID",ComponentTypeEnum::UUID},
			{"SELECTABLE",ComponentTypeEnum::SELECTABLE},
			{"WORLD_MAP",ComponentTypeEnum::WORLD_MAP},
			{"MARKET",ComponentTypeEnum::MARKET},
			{"WORLD_LOCATION",ComponentTypeEnum::WORLD_LOCATION},
			{"JSON_ENTITY",ComponentTypeEnum::JSON_ENTITY},
			{"EXTERN_ENTITY",ComponentTypeEnum::EXTERN_ENTITY},
			{"MAIN_MENU_STATE",ComponentTypeEnum::MAIN_MENU_STATE},
			{"CORE_BG_GAME_STATE",ComponentTypeEnum::CORE_BG_GAME_STATE},
			{"MAIN_GAME_STATE",ComponentTypeEnum::MAIN_GAME_STATE},
			{"MAIN_SETTINGS_STATE",ComponentTypeEnum::MAIN_SETTINGS_STATE},
			{"TACTICAL_STATE",ComponentTypeEnum::TACTICAL_STATE},
			{"BATTLE_STATE",ComponentTypeEnum::BATTLE_STATE},
			{"OVERWORLD_STATE",ComponentTypeEnum::OVERWORLD_STATE},
			{"NEW_GAME_STATE",ComponentTypeEnum::NEW_GAME_STATE},
			{"GAME_STATE_START_HERE",ComponentTypeEnum::GAME_STATE_START_HERE},
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

	inline void to_json(json& j, const ECS_Component& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[p.component_type]},

		};
	}

	inline void from_json(const json& j, ECS_Component& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		j.at("component_type").get_to(component_type_enum);
	}

}