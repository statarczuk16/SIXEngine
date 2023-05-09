
#include <ECS/Core/Coordinator.hpp>
#include <Database.h>
#include <ECS/Systems/BattleSystem.hpp>
#include <ECS/Components/Battle.hpp>

namespace SXNGN::ECS {

	void Battle_System::Init()
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Battle_System Init");
	}

	/// <summary>
	/// Camera's goals: Don't do much, just follow around the target.
	/// </summary>
	/// <param name="dt"></param>
	
	void Battle_System::Update(double dt)
	{
		auto gCoordinator = *Database::get_coordinator();

		//search for battle events
		auto it_act = m_actable_entities.begin();
		while (it_act != m_actable_entities.end())
		{
			auto const& entity_actable = *it_act;
			it_act++;
			auto data = gCoordinator.CheckOutComponent(entity_actable, ComponentTypeEnum::BATTLE);
			if (data)
			{
				Battle* ptr = static_cast< Battle*>(data);
				switch (ptr->battle_state_)
				{
				case BattleState::WAIT_TO_START:
				{
					this->Init_Menus(ptr);
					ptr->battle_state_ = BattleState::LEFT_TURN;
					break;
				}
				case BattleState::LEFT_TURN:
				{
					break;
				}
				case BattleState::RIGHT_TURN:
				{
					break;
				}
				case BattleState::AFTER_ACTION:
				{
					break;
				}
				case BattleState::COMPLETE:
				{
					break;
				}
				}

				gCoordinator.CheckInComponent(entity_actable, ComponentTypeEnum::BATTLE);
			}

		}
	}

	void Battle_System::Init_Menus(Battle* battle)
	{
		auto gCoordinator = Database::get_coordinator();
		std::shared_ptr<SDL_Rect> overworld_viewport = gCoordinator->get_state_manager()->getStateViewPort(ComponentTypeEnum::BATTLE_STATE);
		
		auto ui = UICollectionSingleton::get_instance();
		Entity left_entity = gCoordinator->GetEntityFromUUID(battle->party_left_);
		Entity right_entity = gCoordinator->GetEntityFromUUID(battle->party_right_);

		const Party* left_party = static_cast<const Party*>(gCoordinator->GetComponentReadOnly(left_entity, ComponentTypeEnum::PARTY));
		const Party* right_party = static_cast<const Party*>(gCoordinator->GetComponentReadOnly(right_entity, ComponentTypeEnum::PARTY));
		std::string left_reason;
		std::string right_reason;
		auto left_combat_strength = left_party->get_fighting_strength(left_reason);
		auto right_combat_strength = right_party->get_fighting_strength(right_reason);

		const int WINDOW_HEIGHT = 600;
		const int WINDOW_WIDTH = 400;
		const int TOP_BUFFER = 20;

		const int LABEL_BUFFER_Y = 5;
		const int LABEL_BUFFER_W = 5;
		const int LABEL_WIDTH = 75;
		const int LABEL_HEIGHT = 35;

		const int DEF_NAME_Y = 0 + LABEL_BUFFER_Y;
		const int DEF_VAL_Y = DEF_NAME_Y + LABEL_BUFFER_Y + LABEL_HEIGHT;
		const int ATK_NAME_Y = DEF_VAL_Y + LABEL_BUFFER_Y + LABEL_HEIGHT;
		const int ATK_VAL_Y = ATK_NAME_Y + LABEL_BUFFER_Y + LABEL_HEIGHT;
		const int PARTY_NUMS_NAME = ATK_VAL_Y + LABEL_BUFFER_Y + LABEL_HEIGHT;
		const int PARTY_NUMS_VAL = ATK_VAL_Y + LABEL_BUFFER_Y + LABEL_HEIGHT;

		const int NUM_LABELS = 6;
		
		

		

		auto left_win = UserInputUtils::create_window_raw(nullptr, 0, overworld_viewport->y + TOP_BUFFER, WINDOW_WIDTH, WINDOW_HEIGHT, UILayer::BOTTOM);
		auto right_win = UserInputUtils::create_window_raw(nullptr, overworld_viewport->w - WINDOW_WIDTH, overworld_viewport->y + TOP_BUFFER, WINDOW_WIDTH, WINDOW_HEIGHT, UILayer::BOTTOM);
		
		std::vector<std::shared_ptr<UIContainerComponent>> left_labels;
		std::vector<std::shared_ptr<UIContainerComponent>>  right_labels;

		int row = 0;

		auto left_def_name_c = UserInputUtils::create_label(left_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "Defensive Strength", row++, -1, -1, LABEL_HEIGHT);		
		left_win->child_components_.push_back(left_def_name_c);

		auto left_def_val_c = UserInputUtils::create_label(left_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, " -- Uninit --", row++, -1, -1, LABEL_HEIGHT);
		sprintf(left_def_val_c->label_->text, "%d", left_combat_strength);
		left_win->child_components_.push_back(left_def_val_c);


		auto left_atk_name_c = UserInputUtils::create_label(left_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "Offensive Strength", row++, -1, -1, LABEL_HEIGHT);
		left_win->child_components_.push_back(left_atk_name_c);

		auto left_atk_val_c = UserInputUtils::create_label(left_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, " -- Uninit --", row++, -1, -1, LABEL_HEIGHT);
		sprintf(left_atk_val_c->label_->text, "%d", left_combat_strength);
		left_win->child_components_.push_back(left_atk_val_c);


		auto left_party_nums_name_c = UserInputUtils::create_label(left_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "Party", row++, -1, -1, LABEL_HEIGHT);
		left_win->child_components_.push_back(left_party_nums_name_c);


		auto left_party_nums_val_c = UserInputUtils::create_label(left_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, " -- Uninit --", row++, -1, -1, LABEL_HEIGHT);
		sprintf(left_party_nums_val_c->label_->text, "%.1f", left_party->hands_);
		left_win->child_components_.push_back(left_party_nums_val_c);

		auto left_party_reason_c = UserInputUtils::create_label(left_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, left_reason.data(), row++, -1, -1, LABEL_HEIGHT);
		left_win->child_components_.push_back(left_party_reason_c);
		left_party_reason_c->label_->decorate = 0;

		row = 0;

		auto right_def_name_c = UserInputUtils::create_label(right_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "Defensive Strength", row++, -1, -1, LABEL_HEIGHT);
		right_win->child_components_.push_back(right_def_name_c);

		auto right_def_val_c = UserInputUtils::create_label(right_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, " -- Uninit --", row++, -1, -1, LABEL_HEIGHT);
		sprintf(right_def_val_c->label_->text, "%d", right_combat_strength);
		right_win->child_components_.push_back(right_def_val_c);

		
		auto right_atk_name_c = UserInputUtils::create_label(right_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "Offensive Strength", row++, -1, -1, LABEL_HEIGHT);
		right_win->child_components_.push_back(right_atk_name_c);

		auto right_atk_val_c = UserInputUtils::create_label(right_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, " -- Uninit --", row++, -1, -1, LABEL_HEIGHT);
		sprintf(right_atk_val_c->label_->text, "%d", right_combat_strength);
		right_win->child_components_.push_back(right_atk_val_c);


		auto right_party_nums_name_c = UserInputUtils::create_label(right_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "Party", row++, -1, -1, LABEL_HEIGHT);
		right_win->child_components_.push_back(right_party_nums_name_c);


		auto right_party_nums_val_c = UserInputUtils::create_label(right_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, " -- Uninit --", row++, -1, -1, LABEL_HEIGHT);
		sprintf(right_party_nums_val_c->label_->text, "%.1f", right_party->hands_);
		right_win->child_components_.push_back(right_party_nums_val_c);

		auto right_party_reason_c = UserInputUtils::create_label(right_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, right_reason.data(), row++, -1, -1, LABEL_HEIGHT);
		right_win->child_components_.push_back(right_party_reason_c);
		right_party_reason_c->label_->decorate = 0;

		ui->add_ui_element(ComponentTypeEnum::BATTLE_STATE, left_win);
		ui->add_ui_element(ComponentTypeEnum::BATTLE_STATE, right_win);

		return;

	}
}