
#include <ECS/Core/Coordinator.hpp>
#include <Database.h>
#include <ECS/Systems/BattleSystem.hpp>
#include <ECS/Components/Battle.hpp>
#include <ECS/Utilities/Entity_Builder_Utils.hpp>

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
		auto gCoordinator = Database::get_coordinator();
		std::vector<Entity> entities_to_cleanup;
		//search for battle events
		auto it_act = m_actable_entities.begin();
		while (it_act != m_actable_entities.end())
		{
			auto const& entity_actable = *it_act;
			it_act++;
			auto data = gCoordinator->CheckOutComponent(entity_actable, ComponentTypeEnum::BATTLE);
			if (data)
			{
				Battle* battle = static_cast< Battle*>(data);
				switch (battle->battle_state_)
				{
				case BattleState::WAIT_TO_START:
				{
					this->Init_Menus(battle);
					battle->battle_state_ = BattleState::LEFT_TURN;
					break;
				}
				case BattleState::LEFT_TURN:
				{

					UpdateUI(battle);
					Entity left_entity = gCoordinator->GetEntityFromUUID(battle->party_left_);
					const Party* left_party = static_cast<const Party*>(gCoordinator->GetComponentReadOnly(left_entity, ComponentTypeEnum::PARTY));
					if (left_party->hands_ <= 0)
					{
						battle->battle_state_ = BattleState::DEFEAT;
						break;
					}
					//state change to right turn happens in attack button see init menus
					auto ui = UICollectionSingleton::get_instance();
					ui->string_to_ui_map_["BATTLE_attack_button"]->button_->enabled = 1;

					break;
				}
				case BattleState::RIGHT_TURN:
				{
					UpdateUI(battle);
					Entity right_entity = gCoordinator->GetEntityFromUUID(battle->party_right_);
					const Party* right_party = static_cast<const Party*>(gCoordinator->GetComponentReadOnly(right_entity, ComponentTypeEnum::PARTY));
					if (right_party->hands_ <= 0)
					{
						battle->battle_state_ = BattleState::VICTORY;
						break;
					}
					Enemy_Attack(battle);
					battle->battle_state_ = BattleState::LEFT_TURN;
					break;
				}
				case BattleState::AFTER_ACTION:
				{
					entities_to_cleanup.push_back(entity_actable);
					break;
				}
				case BattleState::WAIT_FOR_GUI:
				{
					
					break;
				}
				case BattleState::ESCAPED:
				{
					auto ui = UICollectionSingleton::get_instance();
					ui->string_to_ui_map_["BATTLE_battle_left"]->cleanup = true;
					ui->string_to_ui_map_["BATTLE_battle_right"]->cleanup = true;
					Event_Component new_game_event;
					new_game_event.e.common.type = EventType::STATE_CHANGE;
					new_game_event.e.state_change.new_states.push_front(ComponentTypeEnum::OVERWORLD_STATE);
					new_game_event.e.state_change.states_to_remove.push_front(ComponentTypeEnum::BATTLE_STATE);
					Entity event_entity = Entity_Builder_Utils::Create_Event(*gCoordinator, ComponentTypeEnum::CORE_BG_GAME_STATE, new_game_event, "Escaped Battle Event");
					battle->battle_state_ = BattleState::AFTER_ACTION;
					break;
				}
				case BattleState::VICTORY:
				{
					UpdateUI(battle);
					SDL_LogInfo(1, "Victory!");
					this->ShowVictoryScreen(battle);
					battle->battle_state_ = BattleState::WAIT_FOR_GUI;
					break;
				}
				case BattleState::DEFEAT:
				{
					UpdateUI(battle);
					//SDL_LogInfo(1, "Game Over");
					Event_Component new_game_event;
					new_game_event.e.common.type = EventType::STATE_CHANGE;
					new_game_event.e.state_change.new_states.push_front(ComponentTypeEnum::OVERWORLD_STATE);
					new_game_event.e.state_change.states_to_remove.push_front(ComponentTypeEnum::BATTLE_STATE);
					Entity event_entity = Entity_Builder_Utils::Create_Event(*gCoordinator, ComponentTypeEnum::CORE_BG_GAME_STATE, new_game_event, "Game Over");
					//battle->battle_state_ = BattleState::AFTER_ACTION;
					break;
				}
				}
				gCoordinator->CheckInComponent(entity_actable, ComponentTypeEnum::BATTLE);
			}

		}

		for (Entity entity : entities_to_cleanup)
		{
			gCoordinator->DestroyEntity(entity);
		}
	}
	
	void Battle_System::ShowVictoryScreen(Battle* battle)
	{
		auto gCoordinator = Database::get_coordinator();
		auto ui = UICollectionSingleton::get_instance();

		//get rid of battle screen
		ui->string_to_ui_map_["BATTLE_battle_left"]->cleanup = true;
		ui->string_to_ui_map_["BATTLE_battle_right"]->cleanup = true;

		

		Entity left_entity = gCoordinator->GetEntityFromUUID(battle->party_left_);
		Party* left_party = static_cast<Party*>(gCoordinator->CheckOutComponent(left_entity, ComponentTypeEnum::PARTY));
		Entity right_entity = gCoordinator->GetEntityFromUUID(battle->party_right_);
		Party* right_party = static_cast<Party*>(gCoordinator->CheckOutComponent(right_entity, ComponentTypeEnum::PARTY));

		std::shared_ptr<SDL_Rect> overworld_viewport = gCoordinator->get_state_manager()->getStateViewPort(ComponentTypeEnum::BATTLE_STATE);

		const int WINDOW_HEIGHT = 600;
		const int WINDOW_WIDTH = 400;
		const int TOP_BUFFER = 20;
		const int LABEL_WIDTH = 75;
		const int LABEL_HEIGHT = 35;

		auto victory_window = UserInputUtils::create_window_raw(nullptr, 0, overworld_viewport->y + TOP_BUFFER, WINDOW_WIDTH, WINDOW_HEIGHT, UILayer::BOTTOM);
		victory_window->name_ = "BATTLE_victory";

		int row = 0;

		auto left_def_name_c = UserInputUtils::create_label(victory_window->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "Victory!", row++, -1, -1, LABEL_HEIGHT);
		victory_window->child_components_.push_back(left_def_name_c);
		auto spoils_c = UserInputUtils::create_label(victory_window->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "Spoils", row++, -1, -1, LABEL_HEIGHT);
		victory_window->child_components_.push_back(spoils_c);

		for (auto item_num_pair : right_party->inventory_)
		{
			if (item_num_pair.second > 0)
			{
				std::string item_name = item_type_to_string()[item_num_pair.first];
				auto item_name_c = UserInputUtils::create_label(victory_window->window_, HA_COLUMN, HA_CENTER, VA_ROW, SP_HALF, UILayer::MID, item_name.data(), row, 0, -1, LABEL_HEIGHT);
				char item_quantity[20] = "";
				sprintf(item_quantity, "%.2f", item_num_pair.second);
				auto item_num_c = UserInputUtils::create_label(victory_window->window_, HA_COLUMN, HA_CENTER, VA_ROW, SP_HALF, UILayer::MID, item_quantity, row++, 1, -1, LABEL_HEIGHT);
				victory_window->child_components_.push_back(item_name_c);
				victory_window->child_components_.push_back(item_num_c);
				left_party->add_item(item_num_pair.first, item_num_pair.second);
			}
		}

		auto exit_battle_c = UserInputUtils::create_button(victory_window->window_, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "Return", row++, 0, -1, LABEL_HEIGHT);
		victory_window->child_components_.push_back(exit_battle_c);

		std::function<void()> kill_window = [victory_window, gCoordinator, battle]()
		{
			victory_window->cleanup = true;

			Event_Component new_game_event;
			new_game_event.e.common.type = EventType::STATE_CHANGE;
			new_game_event.e.state_change.new_states.push_front(ComponentTypeEnum::OVERWORLD_STATE);
			new_game_event.e.state_change.states_to_remove.push_front(ComponentTypeEnum::BATTLE_STATE);
			Entity event_entity = Entity_Builder_Utils::Create_Event(*gCoordinator, ComponentTypeEnum::CORE_BG_GAME_STATE, new_game_event, "End Battle Event");
			battle->battle_state_ = BattleState::AFTER_ACTION;

		};

		exit_battle_c->callback_functions_.push_back(kill_window);


		ui->add_ui_element(ComponentTypeEnum::BATTLE_STATE, victory_window);

		gCoordinator->CheckInComponent(left_entity, ComponentTypeEnum::PARTY);
		gCoordinator->CheckInComponent(right_entity, ComponentTypeEnum::PARTY);


	}

	void Battle_System::Enemy_Attack(Battle* battle)
	{
		auto gCoordinator = Database::get_coordinator();
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(1, 10);

		std::string reason = "";
		Entity left_entity = gCoordinator->GetEntityFromUUID(battle->party_left_);
		Party* left_party = static_cast<Party*>(gCoordinator->CheckOutComponent(left_entity, ComponentTypeEnum::PARTY));
		Entity right_entity = gCoordinator->GetEntityFromUUID(battle->party_right_);
		Party* right_party = static_cast<Party*>(gCoordinator->CheckOutComponent(right_entity, ComponentTypeEnum::PARTY));
		auto attack_strength = left_party->get_fighting_strength(reason);

		int roll1 = dis(gen);
		int roll2 = dis(gen);
		float percent = (roll1 + roll2) / 2.0 / 100.0; //transform 6/10 to 0.06
		//1/3 of attack strength + attack_strenght * random percent
		float damage_val = attack_strength * percent + (attack_strength / 3.0);

		SDL_LogInfo(1, "Enemy attacked for %.2f damage!", damage_val);

		battle->left_health_ -= damage_val;

		if (battle->left_health_ <= 0)
		{
			left_party->hands_ -= 1;
			SDL_LogInfo(1, "Enemy downed a friendly!");
			if (left_party->hands_ <= 0)
			{
				left_party->hands_ = 0;
			}
		}
		gCoordinator->CheckInComponent(right_entity, ComponentTypeEnum::PARTY);
		gCoordinator->CheckInComponent(left_entity, ComponentTypeEnum::PARTY);
		battle->battle_state_ = BattleState::LEFT_TURN;
		
	}

	void Battle_System::UpdateUI(Battle* battle)
	{
		auto gCoordinator = Database::get_coordinator();
		auto ui = UICollectionSingleton::get_instance();
		Entity left_entity = gCoordinator->GetEntityFromUUID(battle->party_left_);
		Entity right_entity = gCoordinator->GetEntityFromUUID(battle->party_right_);

		const Party* left_party = static_cast<const Party*>(gCoordinator->GetComponentReadOnly(left_entity, ComponentTypeEnum::PARTY));
		const Party* right_party = static_cast<const Party*>(gCoordinator->GetComponentReadOnly(right_entity, ComponentTypeEnum::PARTY));

		std::string left_reason;
		std::string right_reason;
		auto left_combat_strength = left_party->get_fighting_strength(left_reason);
		auto right_combat_strength = right_party->get_fighting_strength(right_reason);

		auto ui_comp = ui->string_to_ui_map_["BATTLE_left_health"];
		sprintf(ui_comp->label_->text, "%.2f", battle->left_health_);

		ui_comp = ui->string_to_ui_map_["BATTLE_right_health"];
		sprintf(ui_comp->label_->text, "%.2f", battle->right_health_);

		ui_comp = ui->string_to_ui_map_["BATTLE_left_attack"];
		sprintf(ui_comp->label_->text, "%d", left_combat_strength);

		ui_comp = ui->string_to_ui_map_["BATTLE_right_attack"];
		sprintf(ui_comp->label_->text, "%d", right_combat_strength);

		ui_comp = ui->string_to_ui_map_["BATTLE_left_numbers"];
		sprintf(ui_comp->label_->text, "%.2f", left_party->hands_);

		ui_comp = ui->string_to_ui_map_["BATTLE_right_numbers"];
		sprintf(ui_comp->label_->text, "%.2f", right_party->hands_);

	}

	void Battle_System::Init_Menus(Battle* battle)
	{
		auto gCoordinator = Database::get_coordinator();
		std::shared_ptr<SDL_Rect> overworld_viewport = gCoordinator->get_state_manager()->getStateViewPort(ComponentTypeEnum::BATTLE_STATE);
		
		auto ui = UICollectionSingleton::get_instance();
		Entity left_entity = gCoordinator->GetEntityFromUUID(battle->party_left_);
		Entity right_entity = gCoordinator->GetEntityFromUUID(battle->party_right_);

		Party* left_party = static_cast< Party*>(gCoordinator->CheckOutComponent(left_entity, ComponentTypeEnum::PARTY));
		Party* right_party = static_cast< Party*>(gCoordinator->CheckOutComponent(right_entity, ComponentTypeEnum::PARTY));

		left_party->update_encumbrance();
		right_party->update_encumbrance();

		std::string left_reason;
		std::string right_reason;
		auto left_combat_strength = left_party->get_fighting_strength(left_reason);
		auto right_combat_strength = right_party->get_fighting_strength(right_reason);

		battle->left_health_ = left_combat_strength;
		battle->right_health_ = right_combat_strength;

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
		left_win->name_ = "BATTLE_battle_left";
		auto right_win = UserInputUtils::create_window_raw(nullptr, overworld_viewport->w - WINDOW_WIDTH, overworld_viewport->y + TOP_BUFFER, WINDOW_WIDTH, WINDOW_HEIGHT, UILayer::BOTTOM);
		
		std::vector<std::shared_ptr<UIContainerComponent>> left_labels;
		std::vector<std::shared_ptr<UIContainerComponent>>  right_labels;

		int row = 0;

		auto left_def_name_c = UserInputUtils::create_label(left_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "Defensive Strength", row++, -1, -1, LABEL_HEIGHT);		
		
		left_win->child_components_.push_back(left_def_name_c);

		auto left_def_val_c = UserInputUtils::create_label(left_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, " -- Uninit --", row++, -1, -1, LABEL_HEIGHT);
		left_def_val_c->name_ = "BATTLE_left_health";
		sprintf(left_def_val_c->label_->text, "%.2f", left_combat_strength);
		left_win->child_components_.push_back(left_def_val_c);


		auto left_atk_name_c = UserInputUtils::create_label(left_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "Offensive Strength", row++, -1, -1, LABEL_HEIGHT);
		left_win->child_components_.push_back(left_atk_name_c);

		auto left_atk_val_c = UserInputUtils::create_label(left_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, " -- Uninit --", row++, -1, -1, LABEL_HEIGHT);
		left_atk_val_c->name_ = "BATTLE_left_attack";
		sprintf(left_atk_val_c->label_->text, "%d", left_combat_strength);
		left_win->child_components_.push_back(left_atk_val_c);


		auto left_party_nums_name_c = UserInputUtils::create_label(left_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "Party", row++, -1, -1, LABEL_HEIGHT);
		left_win->child_components_.push_back(left_party_nums_name_c);


		auto left_party_nums_val_c = UserInputUtils::create_label(left_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, " -- Uninit --", row++, -1, -1, LABEL_HEIGHT);
		left_party_nums_val_c->name_ = "BATTLE_left_numbers";
		sprintf(left_party_nums_val_c->label_->text, "%.2f", left_party->hands_);
		left_win->child_components_.push_back(left_party_nums_val_c);

		

		auto attack_c = UserInputUtils::create_button(left_win->window_, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "Attack", row++, -1, -1, LABEL_HEIGHT);
		attack_c->name_ = "BATTLE_attack_button";
		attack_c->button_->enabled = 0;
		left_win->child_components_.push_back(attack_c);

		
		std::function<void()> attack_func = [gCoordinator, battle, attack_c]()
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> dis(1, 10);

			std::string reason = "";
			Entity left_entity = gCoordinator->GetEntityFromUUID(battle->party_left_);
			Party* left_party = static_cast<Party*>(gCoordinator->CheckOutComponent(left_entity, ComponentTypeEnum::PARTY));
			Entity right_entity = gCoordinator->GetEntityFromUUID(battle->party_right_);
			Party* right_party = static_cast<Party*>(gCoordinator->CheckOutComponent(right_entity, ComponentTypeEnum::PARTY));
			auto attack_strength = left_party->get_fighting_strength(reason);

			int roll1 = dis(gen);
			int roll2 = dis(gen);
			float percent = (roll1 + roll2) / 2.0 / 100.0; //transform 6/10 to 0.06
			//1/3 of attack strength + attack_strenght * random percent
			float damage_val = attack_strength * percent + (attack_strength / 3.0);
			SDL_LogInfo(1, "Hit enemies for %.2f damage!", damage_val);
			battle->right_health_ -= damage_val;
			
			if (battle->right_health_ <= 0)
			{
				SDL_LogInfo(1, "Downed an enemy!");
				right_party->hands_ -= 1;
				if (right_party->hands_ <= 0)
				{
					right_party->hands_ = 0;
				}
			}
			gCoordinator->CheckInComponent(right_entity, ComponentTypeEnum::PARTY);
			gCoordinator->CheckInComponent(left_entity, ComponentTypeEnum::PARTY);
			battle->battle_state_ = BattleState::RIGHT_TURN;
			attack_c->button_->enabled = 0;
		};
		attack_c->callback_functions_.push_back(attack_func);

		auto run_c = UserInputUtils::create_button(left_win->window_, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "Run", row++, -1, -1, LABEL_HEIGHT);
		run_c->name_ = "BATTLE_run";

		//Handles running attempts. Success if faster than opponent. Exponential decrease in success for difference between your and enemy speed if you are slower.
		std::function<void()> run_func_2 = [gCoordinator, battle]()
		{
			Entity left_entity = gCoordinator->GetEntityFromUUID(battle->party_left_);
			const Party* left_party = static_cast<const Party*>(gCoordinator->GetComponentReadOnly(left_entity, ComponentTypeEnum::PARTY));
			Entity right_entity = gCoordinator->GetEntityFromUUID(battle->party_right_);
			const Party* right_party = static_cast<const Party*>(gCoordinator->GetComponentReadOnly(right_entity, ComponentTypeEnum::PARTY));

			if (left_party->pace_m_s_ > right_party->pace_m_s_)
			{
				SDL_LogInfo(1, "Escaped! You are faster than enemy.");
				battle->battle_state_ = BattleState::ESCAPED;
				return;
			}
			double speed_disadvantage = right_party->pace_m_s_ / left_party->pace_m_s_;
			double escape_chance = 1 - (1 / (speed_disadvantage * speed_disadvantage));
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> dis(1, 10);
			int roll1 = dis(gen);
			int roll2 = dis(gen);
			float percent = (roll1 + roll2) / 2.0 / 100.0; //transform 6/10 to 0.06
			SDL_LogInfo(1, "Chance to escape: %.2f percent", escape_chance * 100.0);
			if (percent > escape_chance)
			{
				SDL_LogInfo(1, "Escaped!");
				battle->battle_state_ = BattleState::ESCAPED;
				return;
			}
			else
			{
				SDL_LogInfo(1, "Failed to escape!");
				battle->battle_state_ = BattleState::RIGHT_TURN;
			}
		};

		//Open window showing option to run or go back. Show your and enemy speed.
		std::function<void()> run_func_1 = [gCoordinator, battle, left_win, run_func_2, LABEL_HEIGHT]()
		{
			Entity left_entity = gCoordinator->GetEntityFromUUID(battle->party_left_);
			const Party* left_party = static_cast<const Party*>(gCoordinator->GetComponentReadOnly(left_entity, ComponentTypeEnum::PARTY));
			Entity right_entity = gCoordinator->GetEntityFromUUID(battle->party_right_);
			const Party* right_party = static_cast<const Party*>(gCoordinator->GetComponentReadOnly(right_entity, ComponentTypeEnum::PARTY));
			auto run_win_c = UserInputUtils::create_window_raw(left_win->window_, 10, 10, left_win->window_->rect.w, left_win->window_->rect.h, UILayer::TOP);

			std::function<void()> kill_func = [run_win_c]()
			{
				run_win_c->cleanup = true;
			};

			auto confirm_button_c = UserInputUtils::create_button(run_win_c->window_, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::TOP, "Attempt", 0, -1, -1, LABEL_HEIGHT);
			confirm_button_c->callback_functions_.push_back(kill_func);
			confirm_button_c->callback_functions_.push_back(run_func_2);
			run_win_c->child_components_.push_back(confirm_button_c);
			auto reject_button_c = UserInputUtils::create_button(run_win_c->window_, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::TOP, "Back", 1, -1, -1, LABEL_HEIGHT);
			reject_button_c->callback_functions_.push_back(kill_func);
			run_win_c->child_components_.push_back(reject_button_c);

			char flee_text[100];
			sprintf(flee_text, "Try to run?\nYour speed: %.2f\nEnemy speed: %.2f.\n,Abandon equipment to move faster.",left_party->pace_m_s_,right_party->pace_m_s_);

			auto left_party_flee_label = UserInputUtils::create_label(run_win_c->window_, HA_LEFT, HA_LEFT, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::TOP, flee_text, 2, -1, -1, LABEL_HEIGHT);
			left_party_flee_label->label_->decorate = 0;
			run_win_c->child_components_.push_back(left_party_flee_label);


			left_win->child_components_.push_back(run_win_c);
		};
		run_c->callback_functions_.push_back(run_func_1);
		
		left_win->child_components_.push_back(run_c);

		auto talk_c = UserInputUtils::create_button(left_win->window_, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "Talk", row++, -1, -1, LABEL_HEIGHT);
		talk_c->name_ = "BATTLE_talk";
		left_win->child_components_.push_back(talk_c);

		auto left_party_reason_c = UserInputUtils::create_label(left_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, left_reason.data(), row++, -1, -1, LABEL_HEIGHT);
		left_win->child_components_.push_back(left_party_reason_c);
		left_party_reason_c->label_->decorate = 0;

		

		row = 0;

		auto right_def_name_c = UserInputUtils::create_label(right_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "Defensive Strength", row++, -1, -1, LABEL_HEIGHT);
		right_win->child_components_.push_back(right_def_name_c);
		right_win->name_ = "BATTLE_battle_right";

		auto right_def_val_c = UserInputUtils::create_label(right_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, " -- Uninit --", row++, -1, -1, LABEL_HEIGHT);
		right_def_val_c->name_ = "BATTLE_right_health";
		sprintf(right_def_val_c->label_->text, "%.2f", right_combat_strength);
		right_win->child_components_.push_back(right_def_val_c);

		
		auto right_atk_name_c = UserInputUtils::create_label(right_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "Offensive Strength", row++, -1, -1, LABEL_HEIGHT);
		right_win->child_components_.push_back(right_atk_name_c);

		auto right_atk_val_c = UserInputUtils::create_label(right_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, " -- Uninit --", row++, -1, -1, LABEL_HEIGHT);
		right_atk_val_c->name_ = "BATTLE_right_attack";
		sprintf(right_atk_val_c->label_->text, "%d", right_combat_strength);
		right_win->child_components_.push_back(right_atk_val_c);


		auto right_party_nums_name_c = UserInputUtils::create_label(right_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "Party", row++, -1, -1, LABEL_HEIGHT);
		right_win->child_components_.push_back(right_party_nums_name_c);


		auto right_party_nums_val_c = UserInputUtils::create_label(right_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, " -- Uninit --", row++, -1, -1, LABEL_HEIGHT);
		right_party_nums_val_c->name_ = "BATTLE_right_numbers";
		sprintf(right_party_nums_val_c->label_->text, "%.2f", right_party->hands_);
		right_win->child_components_.push_back(right_party_nums_val_c);

		auto right_party_reason_c = UserInputUtils::create_label(right_win->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, right_reason.data(), row++, -1, -1, LABEL_HEIGHT);
		right_win->child_components_.push_back(right_party_reason_c);
		right_party_reason_c->label_->decorate = 0;

		ui->add_ui_element(ComponentTypeEnum::BATTLE_STATE, left_win);
		ui->add_ui_element(ComponentTypeEnum::BATTLE_STATE, right_win);

		gCoordinator->CheckInComponent(right_entity, ComponentTypeEnum::PARTY);
		gCoordinator->CheckInComponent(left_entity, ComponentTypeEnum::PARTY);

		return;

	}
}