#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>
#include <nlohmann/json.hpp>
using nlohmann::json;

namespace SXNGN::ECS {


	enum class BattleState {
		WAIT_TO_START,
		LEFT_TURN,
		RIGHT_TURN,
		AFTER_ACTION,
		VICTORY,
		WAIT_FOR_GUI,
		ESCAPED,
		DEFEAT
	};
	/// <summary>
	/// Contains data displayed when selected
	/// </summary>
	struct Battle : ECS_Component
	{
		Battle()
		{
			component_type = ComponentTypeEnum::BATTLE;
			party_left_ = SXNGN::BAD_UUID;
			party_right_ = SXNGN::BAD_UUID;
			battle_state_ = BattleState::WAIT_TO_START;
			left_health_ = 0;
			right_health_ = 0;
		}
		sole::uuid party_left_;
		sole::uuid party_right_;
		BattleState battle_state_;
		double left_health_; //take damage when this hits 0
		double right_health_; //take damage when this hits 0

	};

	
	inline void to_json(json& j, const Battle& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::BATTLE]},
			{"party_left_",p.party_left_},
			{"party_right_",p.party_right_},
			{"battle_state_",p.battle_state_},
			{"left_health_",p.left_health_},
			{"right_health_",p.right_health_}
		};

	}

	inline void from_json(const json& j, Battle& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		j.at("party_left_").get_to(p.party_left_);
		j.at("party_right_").get_to(p.party_right_);
		j.at("battle_state_").get_to(p.battle_state_);
		j.at("left_health_").get_to(p.left_health_);
		j.at("right_health_").get_to(p.right_health_);
	}
}