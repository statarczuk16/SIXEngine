#include <UI/UserInputUtils.hpp>
#include <vector>
#include <ECS/Core/Coordinator.hpp>
#include <ECS/Components/Components.hpp>

namespace SXNGN::ECS {


	std::shared_ptr<UIContainerComponent> UserInputUtils::create_button(kiss_window* parent_window, h_alignment h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, char* name, int row, int column, int width, int height)
	{
		kiss_button* new_kiss_button = new kiss_button();
		kiss_button_new_uc(new_kiss_button, parent_window, name, 40, 0, width, height);
		new_kiss_button->h_align = h_align;
		new_kiss_button->v_align = v_align;
		new_kiss_button->row = row;
		new_kiss_button->column = column;
		new_kiss_button->parent_scale = parent_scale;
		UIContainerComponent new_ui_container(parent_window, layer, UIType::BUTTON);
		new_ui_container.button_ = new_kiss_button;
		//new_ui_container.name_ = name;
		Event_Component back_state_event_ng;
		return std::make_shared<UIContainerComponent>(new_ui_container);
	}

	std::shared_ptr<UIContainerComponent> UserInputUtils::create_select_button(kiss_window* parent_window, h_alignment h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, char* name, int row, int column, int width, int height)
	{
		kiss_selectbutton* new_kiss_toggle = new kiss_selectbutton();
		kiss_selectbutton_new_uc(new_kiss_toggle, parent_window, 40, 0, width, height);
		new_kiss_toggle->h_align = h_align;
		new_kiss_toggle->v_align = v_align;
		new_kiss_toggle->row = row;
		new_kiss_toggle->column = column;
		new_kiss_toggle->parent_scale = parent_scale;
		new_kiss_toggle->parameter = name;
		UIContainerComponent new_ui_container(parent_window, layer, UIType::SELECT_BUTTON);
		new_ui_container.selectbutton_ = new_kiss_toggle;
		//Event_Component back_state_event_ng;
		return std::make_shared<UIContainerComponent>(new_ui_container);
	}

	std::shared_ptr<UIContainerComponent> UserInputUtils::create_trading_menu(kiss_window* parent_window, std::string title, std::string detail, UILayer layer, TradeHelper* trade_helper)
	{
		//First, use the IDs passed in with the TraderHelper to set the inventories available to trade with
		Coordinator gCoordinator = *SXNGN::Database::get_coordinator();


		std::function<void(std::shared_ptr<UIContainerComponent> component)> kill_ui = [](std::shared_ptr<UIContainerComponent> uicc)
		{
			uicc->cleanup = true;
		};
		
		std::shared_ptr<SDL_Rect> overworld_viewport = gCoordinator.get_state_manager()->getStateViewPort(ComponentTypeEnum::MAIN_GAME_STATE);
		int w = 750;
		int h = 550;
		int window_layer_int = (int)layer;
		int window_item_layer_int = window_layer_int + 1;
		UILayer window_item_layer = (UILayer)window_item_layer_int;
		UILayer window_layer = (UILayer)window_item_layer_int;
		int window_x = overworld_viewport->x + (overworld_viewport->w / 2) - (w / 2);
		int window_y = overworld_viewport->y + (overworld_viewport->h / 2) - (h / 2);
		auto trade_window_c = UserInputUtils::create_window_raw(parent_window, window_x, window_y, w, h, window_layer);
		auto kill_trade_window_func = std::bind(kill_ui, trade_window_c); //function to destroy the trade window

		int item_total_w = 100;
		int item_label_w = 75;
		int amount_edit_w = 75;
		int item_label_h = 30;
		int item_cost_w = 75;
		int add_remove_button_width = 15;
		int player_item_label_x = 10;
		int player_dec_button_x = player_item_label_x + item_label_w + 5;
		int player_amount_edit_x = player_dec_button_x + add_remove_button_width + 5;
		int player_inc_button_x = player_amount_edit_x + amount_edit_w;
		int player_cost_label_x = player_inc_button_x + 5 + add_remove_button_width;
		int player_available_x = player_cost_label_x + 5 + item_cost_w;


		int shop_item_label_x = w - 5 - item_label_w;
		int shop_item_inc_button_x = shop_item_label_x - 5 - add_remove_button_width;
		int shop_amount_edit_x = shop_item_inc_button_x - 5 - amount_edit_w;
		int shop_item_dec_button_x = shop_amount_edit_x - 5 - add_remove_button_width;
		int shop_cost_label_x = shop_item_dec_button_x - 5 - item_cost_w;
		int shop_available_x = shop_cost_label_x - 5 - item_cost_w;

		
		auto confirm_button_c = UserInputUtils::create_button(trade_window_c->window_, HA_COLUMN, VA_ROW, SP_THIRD, window_item_layer, "Confirm", 0, 0, 20, item_label_h);
		confirm_button_c->callback_functions_.push_back(kill_trade_window_func);

		auto quit_button_c = UserInputUtils::create_button(trade_window_c->window_, HA_COLUMN, VA_ROW, SP_THIRD, window_item_layer, "Exit", 0, 2, 20, item_label_h);
		quit_button_c->callback_functions_.push_back(kill_trade_window_func);
		quit_button_c->triggered_events.push_back(gCoordinator.getEvent(SXNGN::ENABLE_UI_SYSTEM_UNPAUSE).first);
		
		

		trade_window_c->child_components_.push_back(quit_button_c);
		trade_window_c->child_components_.push_back(confirm_button_c);

		std::shared_ptr<UIContainerComponent> left_total_label_c = UserInputUtils::create_label(trade_window_c->window_, HA_NONE, HA_CENTER, VA_ROW, SP_NONE, window_item_layer, "0", 0, -1, item_total_w, item_label_h);
		left_total_label_c->label_->rect.x = player_cost_label_x;

		std::shared_ptr<UIContainerComponent> right_total_label_c = UserInputUtils::create_label(trade_window_c->window_, HA_NONE, HA_CENTER, VA_ROW, SP_NONE, window_item_layer, "0", 0, -1, item_total_w, item_label_h);
		right_total_label_c->label_->rect.x = shop_cost_label_x;

		trade_window_c->child_components_.push_back(left_total_label_c);
		trade_window_c->child_components_.push_back(right_total_label_c);

		if(trade_helper->left_is_player)
		{
			
			auto party_data = gCoordinator.GetComponentReadOnly(trade_helper->left_trader, ComponentTypeEnum::PARTY);
			const Party* party_ptr = static_cast<const Party*>(party_data);
			trade_helper->left_inv = party_ptr->inventory_;

		}
		else
		{
			auto market_data = gCoordinator.GetComponentReadOnly(trade_helper->left_trader, ComponentTypeEnum::MARKET);
			const Market* market_ptr = static_cast<const Market*>(market_data);
			trade_helper->left_inv = market_ptr->inventory_;
		}
		auto market_data = gCoordinator.GetComponentReadOnly(trade_helper->right_trader, ComponentTypeEnum::MARKET);
		const Market* market_ptr = static_cast<const Market*>(market_data);
		trade_helper->right_inv = market_ptr->inventory_;

		trade_helper->right_side_total_label = right_total_label_c;
		trade_helper->left_side_total_label = left_total_label_c;
		trade_helper->confirm_button = confirm_button_c;
		
		
		
		//This function is bound to the buttons that change how much of an item is being traded.
		//Eg, for +1 GUN button on the player's side (selling 1 gun), this callback adjusts the total value being traded on the left side and then determines if the store side can afford to buy it and grays out the SELL button accordingly.
		//If on the shop's side, determines whether the player can afford to buy.
		std::function<void(std::shared_ptr<UIContainerComponent> edit_component, ItemType item_type, bool left_1_right_0)>
		 update_total = [trade_helper](std::shared_ptr<UIContainerComponent> edit_component, ItemType item_type, bool left_1_right_0)
		{

			//
			// First, recalculate the total value of goods being sold/bought for what side was modified
			//
			double amount = edit_component->entry_->num_val;
			std::map<ItemType, double> modified_sell_list;
			if (left_1_right_0)
			{
				trade_helper->left_inv_temp[item_type] = amount;
				modified_sell_list = trade_helper->left_inv_temp;
			}
			else
			{
				trade_helper->right_inv_temp[item_type] = amount;
				modified_sell_list = trade_helper->right_inv_temp;
			}
			double modified_running_total = 0.0;
			for (auto item_amount_pair : modified_sell_list)
			{
				modified_running_total += item_type_to_item()[item_amount_pair.first].dec_ * item_amount_pair.second;
			}
			if (left_1_right_0)
			{
				trade_helper->left_running_total = modified_running_total;
				sprintf(trade_helper->left_side_total_label->label_->text, "%.3f KNs", modified_running_total);
			}
			else
			{
				trade_helper->right_running_total = modified_running_total;
				sprintf(trade_helper->right_side_total_label->label_->text, "%.3f KNs", modified_running_total);
			}

			//
			// Second, evaluate the assets on either side and determine if the trade is possible
			//
			double left_assets_total = trade_helper->left_running_total + trade_helper->left_inv[ItemType::KALNOTE] - trade_helper->left_inv_temp[ItemType::KALNOTE];
			double right_assets_total = trade_helper->right_running_total + trade_helper->right_inv[ItemType::KALNOTE] + trade_helper->right_inv_temp[ItemType::KALNOTE];
			//If player is offering more than the store, OK
			if(trade_helper->left_running_total >= trade_helper->right_running_total)
			{
				
				//if amount player is selling is not more than the store's assets, OK
				if(trade_helper->left_running_total <= right_assets_total) 
				{
					trade_helper->confirm_button->button_->enabled = true;
				}
				else
				{
					//seller would approve of transaction, but doesnt have the cash
					trade_helper->confirm_button->button_->enabled = false;
				}
			}
			else
			{
				//seller won't take the bad deal
				trade_helper->confirm_button->button_->enabled = false;
			}
			
		};

		/// Takes all the items traded and puts them into their owner's inventories
		/// Deletes and then remakes the trade window with the updated inventories
		std::function<void()> confirm_trade = [trade_helper]()
		{

			auto gCoordinator = *SXNGN::Database::get_coordinator();

			Party* party_ptr;
			Market* shop_ptr;
			if (trade_helper->left_is_player)
			{

				auto party_data = gCoordinator.CheckOutComponent(trade_helper->left_trader, ComponentTypeEnum::PARTY);
				party_ptr = static_cast<Party*>(party_data);
			}
			
			
			auto market_data = gCoordinator.CheckOutComponent(trade_helper->right_trader, ComponentTypeEnum::MARKET);
			shop_ptr = static_cast<Market*>(market_data);
			

			//all the items the player traded away
			for (auto item_traded : trade_helper->left_inv_temp)
			{
				party_ptr->remove_item(item_traded.first, item_traded.second);
				shop_ptr->add_item(item_traded.first, item_traded.second);
			}
			//all the items the player bought
			for (auto item_traded : trade_helper->right_inv_temp)
			{
				party_ptr->add_item(item_traded.first, item_traded.second);
				shop_ptr->remove_item(item_traded.first, item_traded.second);
			}
			gCoordinator.CheckInComponent(trade_helper->left_trader, ComponentTypeEnum::PARTY);
			gCoordinator.CheckInComponent(trade_helper->right_trader, ComponentTypeEnum::MARKET);
			TradeHelper* trade_helper_new = new TradeHelper();
			trade_helper_new->left_trader = trade_helper->left_trader;
			trade_helper_new->right_trader = trade_helper->right_trader;
			trade_helper_new->left_is_player = trade_helper->left_is_player;
			delete trade_helper;
			auto trading_window = UserInputUtils::create_trading_menu(nullptr, "Trading", "Buy some stuff!", UILayer::BOTTOM, trade_helper_new);
			auto ui = UICollectionSingleton::get_instance();
			ui->add_ui_element(ComponentTypeEnum::OVERWORLD_STATE, trading_window);
			
		};

		confirm_button_c->callback_functions_.push_back(confirm_trade);

		//This callback adjusts the text edit areas when you increment an item to be bought/sold.
		//IE, attach this to the +1 GUN button so the quantity field for GUN updates when you press the button.
		std::function<void(std::shared_ptr<UIContainerComponent> edit_component, float amount)> add_to_edit = [](std::shared_ptr<UIContainerComponent> edit_component, float amount)
		{
			double cur_val = edit_component->entry_->num_val;
			double temp = cur_val + amount;

			switch (edit_component->entry_->entry_type)
			{
			case TE_NONE:
			{
				break;
			}
			case TE_FLOAT:
			{
				
				if (temp >= edit_component->entry_->upper_bound)
				{
					temp = edit_component->entry_->upper_bound;
				}
				else if (temp <= edit_component->entry_->lower_bound)
				{
					temp = edit_component->entry_->lower_bound;
				}
				edit_component->entry_->num_val = temp;
				snprintf(edit_component->entry_->text, KISS_MAX_LENGTH, "%g", edit_component->entry_->num_val);
				break;
			}
			case TE_INT:
			{
				
				if (temp >= edit_component->entry_->upper_bound)
				{
					temp = (int)round(edit_component->entry_->upper_bound);
				}
				else if (temp <= edit_component->entry_->lower_bound)
				{
					temp = (int)round(edit_component->entry_->lower_bound);
				}
				edit_component->entry_->num_val = (int)temp;
				snprintf(edit_component->entry_->text, KISS_MAX_LENGTH, "%d", (int)edit_component->entry_->num_val);
				break;
			}
			default:
			{
				printf("kiss_entry_handle bad entry->entry_type\n");
				abort();
			}
			}
		};

	

		


		

		std::shared_ptr<UIContainerComponent> left_title_label_c = UserInputUtils::create_label(trade_window_c->window_, HA_NONE, HA_CENTER, VA_ROW, SP_NONE, window_item_layer, "Item", 1, -1, item_label_w, item_label_h);
		left_title_label_c->label_->rect.x = player_item_label_x;

		std::shared_ptr<UIContainerComponent> left_amount_label_c = UserInputUtils::create_label(trade_window_c->window_, HA_NONE, HA_CENTER, VA_ROW, SP_NONE, window_item_layer, "To Sell", 1, -1, item_label_w, item_label_h);
		left_amount_label_c->label_->rect.x = player_amount_edit_x;

		std::shared_ptr<UIContainerComponent> left_cost_label_c = UserInputUtils::create_label(trade_window_c->window_, HA_NONE, HA_CENTER, VA_ROW, SP_NONE, window_item_layer, "Sell Price", 1, -1, item_cost_w, item_label_h);
		left_cost_label_c->label_->rect.x = player_cost_label_x;

		std::shared_ptr<UIContainerComponent> player_stock_label_c = UserInputUtils::create_label(trade_window_c->window_, HA_NONE, HA_CENTER, VA_ROW, SP_NONE, window_item_layer, "Stock", 1, -1, item_label_w, item_label_h);
		player_stock_label_c->label_->rect.x = player_available_x;

		std::shared_ptr<UIContainerComponent> right_title_label_c = UserInputUtils::create_label(trade_window_c->window_, HA_NONE, HA_CENTER, VA_ROW, SP_NONE, window_item_layer, "Item", 1, -1, item_label_w, item_label_h);
		right_title_label_c->label_->rect.x = shop_item_label_x;

		std::shared_ptr<UIContainerComponent> right_amount_label_c = UserInputUtils::create_label(trade_window_c->window_, HA_NONE, HA_CENTER, VA_ROW, SP_NONE, window_item_layer, "To Buy", 1, -1, item_label_w, item_label_h);
		right_amount_label_c->label_->rect.x = shop_amount_edit_x;

		std::shared_ptr<UIContainerComponent> right_cost_label_c = UserInputUtils::create_label(trade_window_c->window_, HA_NONE, HA_CENTER, VA_ROW, SP_NONE, window_item_layer, "Buy Price", 1, -1, item_cost_w, item_label_h);
		right_cost_label_c->label_->rect.x = shop_cost_label_x;

		std::shared_ptr<UIContainerComponent> right_stock_label_c = UserInputUtils::create_label(trade_window_c->window_, HA_NONE, HA_CENTER, VA_ROW, SP_NONE, window_item_layer, "Stock", 1, -1, item_label_w, item_label_h);
		right_stock_label_c->label_->rect.x = shop_available_x;

		trade_window_c->child_components_.push_back(left_title_label_c);
		trade_window_c->child_components_.push_back(left_amount_label_c);
		trade_window_c->child_components_.push_back(left_cost_label_c);
		trade_window_c->child_components_.push_back(player_stock_label_c);

		trade_window_c->child_components_.push_back(right_title_label_c);
		trade_window_c->child_components_.push_back(right_amount_label_c);
		trade_window_c->child_components_.push_back(right_cost_label_c);
		trade_window_c->child_components_.push_back(right_stock_label_c);


		

		
		int row = 2;
		for (auto item : trade_helper->left_inv)
		{
			double amt_available = item.second;
			if (amt_available <= 0)
			{
				continue;
			}
			std::string item_name = item_type_to_string()[item.first];
			double base_cost = item_type_to_item()[item.first].base_value_kn_;
			std::shared_ptr<UIContainerComponent> left_title_label_c = UserInputUtils::create_label(trade_window_c->window_, HA_NONE, HA_CENTER, VA_ROW, SP_NONE, window_item_layer, item_name.data(), row, -1, item_label_w, item_label_h);
			left_title_label_c->label_->rect.x = player_item_label_x;
			
			std::shared_ptr<UIContainerComponent> left_decrease_button_c = UserInputUtils::create_button(trade_window_c->window_, HA_NONE, VA_ROW, SP_NONE, window_item_layer, "-", row, -1, add_remove_button_width, item_label_h);
			left_decrease_button_c->button_->rect.x = player_dec_button_x;
			
			std::shared_ptr<UIContainerComponent> left_increase_button_c = UserInputUtils::create_button(trade_window_c->window_, HA_NONE, VA_ROW, SP_NONE, window_item_layer, "+", row, -1, add_remove_button_width, item_label_h);
			left_increase_button_c->button_->rect.x = player_inc_button_x;

			std::shared_ptr<UIContainerComponent> left_amount_entry_c = UserInputUtils::create_num_entry(trade_window_c->window_, HA_NONE, VA_ROW, SP_NONE, window_item_layer, "?", 0, amt_available, TE_INT, 0, row, -1);
			left_amount_entry_c->entry_->rect.x = player_amount_edit_x;
			left_amount_entry_c->entry_->rect.w = amount_edit_w;
			left_amount_entry_c->entry_->rect.h = item_label_h;
			char base_cost_str[10];
			sprintf(base_cost_str, "%.3f", base_cost);
			std::shared_ptr<UIContainerComponent> left_cost_label_c = UserInputUtils::create_label(trade_window_c->window_, HA_NONE, HA_CENTER, VA_ROW, SP_NONE, window_item_layer, base_cost_str, row, -1, item_cost_w, item_label_h);
			left_cost_label_c->label_->rect.x = player_cost_label_x;

			std::shared_ptr<UIContainerComponent> left_stock_label_c = UserInputUtils::create_label(trade_window_c->window_, HA_NONE, HA_CENTER, VA_ROW, SP_NONE, window_item_layer, "", row, -1, item_label_w, item_label_h);
			left_stock_label_c->label_->rect.x = player_available_x;
			sprintf(left_stock_label_c->label_->text, "%.2f", amt_available);

			left_decrease_button_c->callback_functions_.push_back(std::bind(add_to_edit, left_amount_entry_c, -1));
			left_increase_button_c->callback_functions_.push_back(std::bind(add_to_edit, left_amount_entry_c, 1));
			left_increase_button_c->callback_functions_.push_back(std::bind(update_total, left_amount_entry_c,  item.first, true));
			left_decrease_button_c->callback_functions_.push_back(std::bind(update_total, left_amount_entry_c, item.first, true));
			left_amount_entry_c->callback_functions_.push_back(std::bind(update_total, left_amount_entry_c,  item.first, true));

			trade_window_c->child_components_.push_back(left_title_label_c);
			trade_window_c->child_components_.push_back(left_decrease_button_c);
			trade_window_c->child_components_.push_back(left_increase_button_c);
			trade_window_c->child_components_.push_back(left_cost_label_c);
			trade_window_c->child_components_.push_back(left_amount_entry_c);
			trade_window_c->child_components_.push_back(left_stock_label_c);

			
			
			row++;
		}
		int left_max_row = row;

		row = 2;
		for (auto item : trade_helper->right_inv)
		{

			double amt_available = item.second;
			if (amt_available <= 0)
			{
				continue;
			}
			double base_cost = item_type_to_item()[item.first].base_value_kn_;
			std::string item_name = item_type_to_string()[item.first];
			std::shared_ptr<UIContainerComponent> right_title_label_c = UserInputUtils::create_label(trade_window_c->window_, HA_NONE, HA_CENTER, VA_ROW, SP_NONE, window_item_layer, item_name.data(), row, -1, item_label_w, item_label_h);
			right_title_label_c->label_->rect.x = shop_item_label_x;

			std::shared_ptr<UIContainerComponent> right_decrease_button_c = UserInputUtils::create_button(trade_window_c->window_, HA_NONE, VA_ROW, SP_NONE, window_item_layer, "-", row, -1, add_remove_button_width, item_label_h);
			right_decrease_button_c->button_->rect.x = shop_item_dec_button_x;
			std::shared_ptr<UIContainerComponent> right_increase_button_c = UserInputUtils::create_button(trade_window_c->window_, HA_NONE, VA_ROW, SP_NONE, window_item_layer, "+", row, -1, add_remove_button_width, item_label_h);
			right_increase_button_c->button_->rect.x = shop_item_inc_button_x;
			std::shared_ptr<UIContainerComponent> right_amount_entry_c = UserInputUtils::create_num_entry(trade_window_c->window_, HA_NONE, VA_ROW, SP_NONE, window_item_layer, "?", 0, amt_available, TE_INT, 0, row, -1);
			right_amount_entry_c->entry_->rect.x = shop_amount_edit_x;
			right_amount_entry_c->entry_->rect.w = amount_edit_w;
			right_amount_entry_c->entry_->rect.h = item_label_h;
			char base_cost_str[10];
			sprintf(base_cost_str, "%.3f", base_cost);
			std::shared_ptr<UIContainerComponent> right_cost_label_c = UserInputUtils::create_label(trade_window_c->window_, HA_NONE, HA_CENTER, VA_ROW, SP_NONE, window_item_layer, base_cost_str, row, -1, item_cost_w, item_label_h);
			right_cost_label_c->label_->rect.x = shop_cost_label_x;

			std::shared_ptr<UIContainerComponent> right_stock_label_c = UserInputUtils::create_label(trade_window_c->window_, HA_NONE, HA_CENTER, VA_ROW, SP_NONE, window_item_layer, "", row, -1, item_label_w, item_label_h);
			right_stock_label_c->label_->rect.x = shop_available_x;
			sprintf(right_stock_label_c->label_->text, "%.2f", amt_available);

			right_decrease_button_c->callback_functions_.push_back(std::bind(add_to_edit, right_amount_entry_c, -1));
			right_increase_button_c->callback_functions_.push_back(std::bind(add_to_edit, right_amount_entry_c, 1));
			right_increase_button_c->callback_functions_.push_back(std::bind(update_total, right_amount_entry_c,  item.first, false));
			right_decrease_button_c->callback_functions_.push_back(std::bind(update_total, right_amount_entry_c, item.first, false));
			right_amount_entry_c->callback_functions_.push_back(std::bind(update_total, right_amount_entry_c,  item.first, false));

			trade_window_c->child_components_.push_back(right_title_label_c);
			trade_window_c->child_components_.push_back(right_decrease_button_c);
			trade_window_c->child_components_.push_back(right_increase_button_c);
			trade_window_c->child_components_.push_back(right_cost_label_c);
			trade_window_c->child_components_.push_back(right_amount_entry_c);
			trade_window_c->child_components_.push_back(right_stock_label_c);
			row++;
		}

		if (left_max_row > row)
		{
			row = left_max_row;
		}
		confirm_button_c->button_->row = row + 1;
		left_total_label_c->label_->row = row;
		quit_button_c->button_->row = row + 1;
		right_total_label_c->label_->row = row;
	
		

		

		

		return trade_window_c;
	}
		

	std::shared_ptr<UIContainerComponent> UserInputUtils::create_message_box(kiss_window* parent_window, std::string title, std::string detail, int w, int h, UILayer layer, std::vector<std::string> option_strings, std::vector<std::function<void()>> option_callbacks, std::vector<bool> option_enables)
	{
		Coordinator gCoordinator = *SXNGN::Database::get_coordinator();
		std::shared_ptr<SDL_Rect> overworld_viewport = gCoordinator.get_state_manager()->getStateViewPort(ComponentTypeEnum::MAIN_GAME_STATE);
		int window_layer_int = (int)layer;
		int window_item_layer_int = window_layer_int;
		UILayer window_item_layer = (UILayer)window_item_layer_int;
		int window_x = overworld_viewport->x + (overworld_viewport->w / 2) - (w / 2);
		int window_y = overworld_viewport->y + (overworld_viewport->h / 2) - (h / 2);
		auto message_window_c = UserInputUtils::create_window_raw(parent_window, window_x, window_y, w, h, layer);
		int column = 0;
		int row = 0;
		char* title_str = title.data();
		std::shared_ptr<UIContainerComponent> title_label_c = UserInputUtils::create_label(message_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, window_item_layer, title_str, row++, -1, 50, 50);
		char* detail_str = detail.data();
		std::shared_ptr<UIContainerComponent> detail_label_c = UserInputUtils::create_label(message_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, window_item_layer, detail_str, row++, -1, 50, 50);
		message_window_c->child_components_.push_back(title_label_c);
		message_window_c->child_components_.push_back(detail_label_c);


		row = 3;
		column = 0;
		scale_to_parent_width button_width = SP_HALF;
		h_alignment h_align = HA_COLUMN;
		switch (option_strings.size())
		{
		case 1: button_width = SP_FILL_WITH_BUFFER; h_align = HA_CENTER; break;
		case 2: button_width = SP_HALF; break;
		case 3: button_width = SP_THIRD; break;
		case 4: button_width = SP_FOURTH; break;
		default: button_width = SP_FOURTH; break;
		}
		for (int i = 0; i < option_strings.size(); i++)
		{
			column = i % 4;
			char* button_str = option_strings[i].data();
			std::shared_ptr<UIContainerComponent> button_c = UserInputUtils::create_button(message_window_c->window_, h_align, VA_ROW, button_width, window_item_layer, button_str, row, column, 50, 50);

			//set button to disabled if does not pass the test passed in
			if (option_enables.size() > i)
			{
				if (!option_enables[i])
				{
					button_c->button_->enabled = false;
				}
			}
			std::function<void(std::shared_ptr<UIContainerComponent> component)> kill_ui = [](std::shared_ptr<UIContainerComponent> uicc)
			{
				uicc->cleanup = true;
			};
			auto kill_button = std::bind(kill_ui, message_window_c);
			button_c->callback_functions_.push_back(kill_button);
			button_c->callback_functions_.push_back(option_callbacks[i]);
			button_c->triggered_events.push_back(gCoordinator.getEvent(SXNGN::ENABLE_UI_SYSTEM_UNPAUSE).first);
			message_window_c->child_components_.push_back(button_c);
		
		}
		


		return message_window_c;

	}


	std::shared_ptr<UIContainerComponent> UserInputUtils::create_window_raw(kiss_window* parent_window, int x, int y, int w, int h, UILayer layer)
	{

		
		kiss_window* new_kiss_window = new kiss_window();
		kiss_window_new(new_kiss_window, parent_window, 1, x, y, w, h);
		new_kiss_window->visible = true;
		UIContainerComponent mmw_container(parent_window, layer, UIType::WINDOW);
		mmw_container.window_ = new_kiss_window;
		return std::make_shared<UIContainerComponent>(mmw_container);
	}

	std::shared_ptr<UIContainerComponent> UserInputUtils::create_label(kiss_window* parent_window, h_alignment h_align, h_alignment txt_h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, char* name, int row, int column, int width, int height)
	{
		kiss_label* new_kiss_label = new kiss_label();
		kiss_label_new_uc(new_kiss_label, parent_window, name, 0, 0, width, height);
		new_kiss_label->h_align = h_align;//label center to parent window window
		new_kiss_label->v_align = v_align;
		new_kiss_label->txt_h_align = txt_h_align;//text center to this label
		new_kiss_label->txt_v_align = v_alignment::VA_CENTER;//text center to this label
		new_kiss_label->row = row;
		new_kiss_label->column = column;
		new_kiss_label->parent_scale = parent_scale;
		UIContainerComponent ngl_c(parent_window, layer, UIType::LABEL);
		ngl_c.label_ = new_kiss_label;
		return std::make_shared<UIContainerComponent>(ngl_c);

	}


	std::shared_ptr<UIContainerComponent> UserInputUtils::create_progressbar_from_callback(kiss_window* parent_window, h_alignment h_align, h_alignment txt_h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, std::function<double()> callback, int row, int column, int width, int height)
	{
		//TODO
		kiss_progressbar* new_kiss_progress = new kiss_progressbar();
		kiss_progressbar_new_uc(new_kiss_progress, parent_window, 0, 0, width, height);
		new_kiss_progress->h_align = h_align;//label center to parent window window
		new_kiss_progress->v_align = v_align;
		new_kiss_progress->txt_h_align = txt_h_align;//text center to this label
		new_kiss_progress->row = row;
		new_kiss_progress->column = column;
		new_kiss_progress->parent_scale = parent_scale;
		UIContainerComponent ngl_c(parent_window, layer, UIType::PROGRESSBAR);
		ngl_c.progressbar_ = new_kiss_progress;
		ngl_c.property_ = "";
		return std::make_shared<UIContainerComponent>(ngl_c);
	}


	std::shared_ptr<UIContainerComponent> UserInputUtils::create_progressbar_from_property(kiss_window* parent_window, h_alignment h_align, h_alignment txt_h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, std::string prop, int row, int column, int width, int height)
	{
		kiss_progressbar* new_kiss_progress = new kiss_progressbar();
		kiss_progressbar_new_uc(new_kiss_progress, parent_window, 0, 0, width, height);
		new_kiss_progress->h_align = h_align;//label center to parent window window
		new_kiss_progress->v_align = v_align;
		new_kiss_progress->txt_h_align = txt_h_align;//text center to this label
		new_kiss_progress->row = row;
		new_kiss_progress->column = column;
		new_kiss_progress->parent_scale = parent_scale;
		UIContainerComponent ngl_c(parent_window, layer, UIType::PROGRESSBAR);
		ngl_c.progressbar_ = new_kiss_progress;
		ngl_c.property_ = prop;
		return std::make_shared<UIContainerComponent>(ngl_c);
	}

	std::shared_ptr<UIContainerComponent> UserInputUtils::create_text_entry(kiss_window* parent_window, h_alignment h_align, v_alignment v_align, scale_to_parent_width parent_scale, UILayer layer, char* name, int row, int column)
	{
		kiss_entry* new_kiss_entry = new kiss_entry();
		kiss_entry_new(new_kiss_entry, parent_window, 1, "", 0, 0, 400);
		new_kiss_entry->h_align = h_align;
		new_kiss_entry->v_align = v_align;
		new_kiss_entry->row = row;
		new_kiss_entry->column = column;
		new_kiss_entry->parent_scale = parent_scale;
		new_kiss_entry->lower_bound = 2;
		new_kiss_entry->upper_bound = 24;
		new_kiss_entry->num_val = 2;
		new_kiss_entry->entry_type = TE_NONE;
		new_kiss_entry->rect.h = 50;

		UIContainerComponent new_container(parent_window, layer, UIType::ENTRY);
		new_container.entry_ = new_kiss_entry;
		return std::make_shared<UIContainerComponent>(new_container);
	}

	std::shared_ptr<UIContainerComponent> UserInputUtils::create_num_entry(kiss_window* parent_window, h_alignment h_align, v_alignment v_align, scale_to_parent_width parent_scale,
		UILayer layer, char* name, double lower_bound, double upper_bound, text_entry_type entry_type, double default_val, int row, int column)
	{
		auto new_container = UserInputUtils::create_text_entry(parent_window, h_align, v_align, parent_scale, layer, name, row, column);
		new_container->entry_->lower_bound = lower_bound;
		new_container->entry_->upper_bound = upper_bound;
		new_container->entry_->num_val = default_val;
		snprintf(new_container->entry_->text, 260, "%g", default_val);
		new_container->entry_->entry_type = entry_type;
		return new_container;
		
	}

	/// <summary>
	/// Sort a vector of SDL events into only the ones the game cares about
	/// </summary>
	/// <param name="events"></param>
	/// <returns></returns>
	std::vector<std::vector<SDL_Event>> UserInputUtils::filter_sdl_events(std::vector<SDL_Event> events)
	{
		std::vector<SDL_Event> mouse_events;
		std::vector<SDL_Event> keyboard_events;
		std::vector<SDL_Event> mouse_wheel_events;

		for (auto e : events)
		{
			if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
			{
				mouse_events.push_back(e);
			}
			else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
			{
				keyboard_events.push_back(e);
				//mouse events interested in these modify types
				switch (e.key.keysym.sym)
				{
				case SDLK_LSHIFT: mouse_events.push_back(e); break;
				case SDLK_LALT: mouse_events.push_back(e); break;
				case SDLK_LCTRL: mouse_events.push_back(e); break;
				}
			}
			else if(e.type == SDL_MOUSEWHEEL)
			{
				mouse_wheel_events.push_back(e);
			}
		}

		std::vector<std::vector<SDL_Event>> ret = { mouse_events, keyboard_events, mouse_wheel_events };
		return ret;
	}

	std::pair<Sint32, Sint32> UserInputUtils::wasd_to_x_y(std::vector<SDL_Event> events)
	{
		Sint32 x = 0;
		Sint32 y = 0;
		Sint32 add_amount = 1;
		for (auto event : events)
		{
			auto e = event;
			if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
			{
				//Adjust the velocity
				switch (e.key.keysym.sym)
				{
				case SDLK_UP: y -= add_amount; break;
				case SDLK_DOWN: y += add_amount; break;
				case SDLK_LEFT: x -= add_amount; break;
				case SDLK_RIGHT: x += add_amount; break;
				case SDLK_LSHIFT: add_amount = 2; break; //go twice as fast if holding shift
				}
			}

			if (e.type == SDL_KEYUP && e.key.repeat == 0)
			{
				//Adjust the velocity
				switch (e.key.keysym.sym)
				{
				case SDLK_UP: y += add_amount; break;
				case SDLK_DOWN: y -= add_amount; break;
				case SDLK_LEFT: x += add_amount; break;
				case SDLK_RIGHT: x -= add_amount; break;
				case SDLK_LSHIFT: add_amount = 1; break;
				}
			}
		}
		return std::make_pair(x, y);

	}

	/**
	 UserInputUtils::mouse_movement_to_mouse_state(std::vector<SDL_Event> events)
	{
		size_t x = 0.0;
		size_t y = 0.0;
		size_t add_amount = 1;
		for (auto event : events)
		{
			auto e = event;
			if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
			{
				//Adjust the velocity
				switch (e.key.keysym.sym)
				{
				case SDLK_UP: y -= add_amount; break;
				case SDLK_DOWN: y += add_amount; break;
				case SDLK_LEFT: x -= add_amount; break;
				case SDLK_RIGHT: x += add_amount; break;
				case SDLK_LSHIFT: add_amount = 2; break; //go twice as fast if holding shift
				}
			}

			if (e.type == SDL_KEYUP && e.key.repeat == 0)
			{
				//Adjust the velocity
				switch (e.key.keysym.sym)
				{
				case SDLK_UP: y += add_amount; break;
				case SDLK_DOWN: y -= add_amount; break;
				case SDLK_LEFT: x += add_amount; break;
				case SDLK_RIGHT: x -= add_amount; break;
				case SDLK_LSHIFT: add_amount = 1; break;
				}
			}
		}
		return std::make_pair(x, y);

	} **/
}