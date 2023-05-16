#include <ECS/Components/Party.hpp>
#include <ECS/Components/UI.hpp>

namespace SXNGN::ECS {

	Party::Party()
	{
		component_type = ComponentTypeEnum::PARTY;
		health_ = 100.0;
		health_max_ = 100.0;
		stamina_ = 1000.0;
		stamina_max_ = stamina_;
		pace_abs_ = 0.0;

		hands_ = 1.0;
		lost_counter_ = 0.0;
		sick_counter_s_ = 0.0;
		weather_counter_s_ = 0.0;
		lost_counter_max_ = 0.0;
		sick_counter_max_ = 0.0;
		weather_counter_max_ = 0.0;
		sick_level_ = EventSeverity::UNKNOWN;
		weather_level_ = EventSeverity::UNKNOWN;
		encumbrance_penalty_m_s_ = 0.0;



		bool at_ruins_ = false;
		bool at_settlement_ = false;
		inventory_[ItemType::AMMO] = 0;
		inventory_[ItemType::BATTERY] = 0;
		inventory_[ItemType::FOOD] = 1;
		inventory_[ItemType::FOOTWEAR] = 1;
		inventory_[ItemType::GPS] = 0;
		inventory_[ItemType::GUN] = 0;
		inventory_[ItemType::MEDKIT] = 0;
		inventory_[ItemType::WATER] = 0;
		update_encumbrance();
	}

	void Party::update_player_ui()
	{
		auto ui = UICollectionSingleton::get_instance();
		auto weight_val_real = ui->string_to_ui_map_["OVERWORLD_inventoryweight_value_real"];
		auto weight_val = ui->string_to_ui_map_["OVERWORLD_inventoryweight_value"];
		auto weight_progress_bar = ui->string_to_ui_map_["OVERWORLD_progress_encumber"];
		weight_progress_bar->progressbar_->value = encumbrance_real_kg_;
		weight_progress_bar->progressbar_->max_value = weight_capacity_kg_;
		snprintf(weight_val_real->label_->text, 100, "%.2f", encumbrance_real_kg_);
		snprintf(weight_val->label_->text, 100, "%.2f", encumbrance_kg_);
	}

	void Party::update_encumbrance()
	{
		encumbrance_kg_ = 0.0;
		weight_capacity_kg_ = hands_ * PARTY_WEIGHT_CAPACITY_PER_HAND_KG;
		for (auto item_i : inventory_)
		{
			double item_weight = item_type_to_item()[item_i.first].weight_kg_;
			encumbrance_kg_ += item_i.second * item_weight;
		}
		update_encumbrance_threshs();
	}

	void Party::update_encumbrance_threshs()
	{
		
		overencumbered_mild_thresh_kg_ = weight_capacity_kg_ * 0.70;
		overencumbered_medium_thresh_kg_ = weight_capacity_kg_ * 0.85;
		overencumbered_extreme_thresh_kg_ = weight_capacity_kg_;
		double encumb_temp = encumbrance_kg_;

		if (sick_level_ == EventSeverity::UNKNOWN)
		{
			encumb_temp = encumb_temp;
		}
		else if (sick_level_ == EventSeverity::MILD)
		{
			encumb_temp += encumb_temp * PARTY_SICK_ENCUMBER_MULTIPLY_MILD;
		}
		else if (sick_level_ == EventSeverity::MEDIUM)
		{
			encumb_temp += encumb_temp * PARTY_SICK_ENCUMBER_MULTIPLY_MED;
		}
		else if (sick_level_ == EventSeverity::EXTREME)
		{
			encumb_temp += encumb_temp * PARTY_SICK_ENCUMBER_MULTIPLY_EXT;
		}
		else if (sick_level_ == EventSeverity::SPICY)
		{
			encumb_temp += encumb_temp * PARTY_SICK_ENCUMBER_MULTIPLY_SPICY;
		}
		
		if (encumb_temp > overencumbered_mild_thresh_kg_)
		{
			encumbrance_penalty_m_s_ = SXNGN::PARTY_PACE_WEIGHT_PENALTY_MILD;
		}
		if (encumb_temp > overencumbered_medium_thresh_kg_)
		{
			encumbrance_penalty_m_s_ = SXNGN::PARTY_PACE_WEIGHT_PENALTY_MEDIUM;
		}
		if (encumb_temp > overencumbered_extreme_thresh_kg_)
		{
			encumbrance_penalty_m_s_ = SXNGN::PARTY_PACE_WEIGHT_PENALTY_EXTREME;
		}

		encumbrance_real_kg_ = encumb_temp;
		pace_m_s_ = SXNGN::PARTY_PACE_NOMINAL_M_S - encumbrance_penalty_m_s_;
		
	}

	void Party::add_item(ItemType item, double amount)
	{
		if (inventory_.count(item) > 0)
		{
			inventory_[item] += amount;
		}
		else
		{
			inventory_[item] = amount;
		}
		encumbrance_kg_ += amount * item_type_to_item()[item].weight_kg_;
		update_encumbrance_threshs();
	}

	void Party::remove_item(ItemType item, double amount)
	{
		if (inventory_.count(item) > 0)
		{
			inventory_[item] -= amount;
			if (inventory_[item] < 0)
			{
				inventory_.erase(item);
			}
			encumbrance_kg_ -= amount * item_type_to_item()[item].weight_kg_;
			update_encumbrance_threshs();
		}

	}

	bool Party::can_use_gps(std::string& result)
	{
		if (inventory_[ItemType::BATTERY] > 0 && inventory_[ItemType::GPS] > 0)
		{

			result = "Success";
			return true;
		}
		else if (inventory_[ItemType::GPS] < 1)
		{
			result = "No GPS";
		}
		else
		{
			result = "Need batteries";
		}
		return false;
	}

	
	int Party::use_gps(std::string& result)
	{

		if (can_use_gps(result))
		{
			if (lost_counter_ > 0.0)
			{
				lost_counter_ = 0.0;
				remove_item(ItemType::BATTERY, 1);
				result = "Success";
				return 0;
			}
			else
			{
				result = "Not Lost";
				return 1;
			}
		}
		else
		{
			//result already filled in from can_use_gps
			return 1;
		}
	}

	int Party::use_medkit(std::string& result)
	{
		if (can_use_medit(result))
		{
			if (sick_counter_s_ > 0.0)
			{
				result = "Cured sickness with medkit.";
				sick_counter_s_ = 0.0;
				remove_item(ItemType::MEDKIT, 1);
				return 0;
			}
			else if (health_ < health_max_)
			{
				health_ = health_max_;
				remove_item(ItemType::MEDKIT, 1);
				result = "Recovered health with medkit.";
				return 0;
			}
		}
		else
		{
			//result filled in from can_use_medkit
			return 1;
		}
	}

	bool Party::can_use_medit(std::string& result)
	{
		if (inventory_[ItemType::MEDKIT] > 0)
		{
			result = "Success";
			return true;
		}
		else
		{
			result = "No Medkits";
			return false;
		}
	}

	bool Party::have_footwear(std::string& result)
	{
		return inventory_[ItemType::FOOTWEAR] >= hands_;
	}

	int Party::hands_without_footwear(std::string& result)
	{
		//10 guys and 3 boots = 7 without boots
		//10 guys and 12 boots = -2 without boots
		int temp = hands_ - inventory_[ItemType::FOOTWEAR];
		if (temp > 0)
		{
			return temp;
		}
		else
		{
			return 0;
		}
	}

	int Party::use_item(ItemType item, std::string& result)
	{
		switch (item)
		{
		case ItemType::AMMO:
		{
			result = "Unuseable Item";
			return 1;
			break;
		}
		case ItemType::BATTERY:
		{
			result = "Unuseable Item";
			return 1;
			break;
		}
		case ItemType::FOOD:
		{
			result = "Unuseable Item";
			return 1;
			break;
		}
		case ItemType::FOOTWEAR:
		{
			result = "Footwear worn automatically.";
			return 1;
			break;
		}
		case ItemType::GPS:
		{
			if (can_use_gps(result))
			{
				return use_gps(result);
			}
			break;
		}
		case ItemType::GUN:
		{
			result = "Can't use outside combat.";
			return 1;
			break;
		}
		case ItemType::KALNOTE:
		{
			result = "Can't use outside trading.";
			return 1;
			break;
		}
		case ItemType::MEDKIT:
		{
			if (can_use_medit(result))
			{
				return can_use_medit(result);
			}
			break;
		}
		case ItemType::SCRAP:
		{
			result = "Can't use outside trading.";
			return 1;
			break;
		}
		default:
		{
			result = "Unuseable Item";
			return 1;
			break;
		}
		}
	}

	bool Party::can_use_item(ItemType item, std::string& result)
	{
		switch (item)
		{
			case ItemType::AMMO:
			{
				result = "Unable Item";
				return false;
				break;
			}
			case ItemType::BATTERY:
			{
				result = "Unable Item";
				return false;
				break;
			}
			case ItemType::FOOD:
			{
				result = "Rest to eat.";
				return false;
				break;
			}
			case ItemType::FOOTWEAR:
			{
				result = "Footwear worn automatically.";
				return false;
				break;
			}
			case ItemType::GPS:
			{
				return can_use_gps(result);
				break;
			}
			case ItemType::GUN:
			{
				result = "Can't use outside combat.";
				return false;
				break;
			}
			case ItemType::KALNOTE:
			{
				result = "Can't use outside trading.";
				return false;
				break;
			}
			case ItemType::MEDKIT:
			{
				return can_use_medit(result);
				break;
			}
			case ItemType::SCRAP:
			{
				result = "Can't use outside trading.";
				return false;
				break;
			}
			default:
			{
				result = "Unable Item";
				return false;
				break;
			}
		}
	}

	int Party::get_fighting_strength(std::string& result) const
	{
		//hand with a gun = 10 strength
		//hand with no gun = 2 strength
		//gun needs at least 1 bullet per combat round to fight

		int strength = 0;
		int guns_temp = 0;
		result = "Fighting Strength\n";
		if (inventory_.at(ItemType::AMMO) > inventory_.at(ItemType::GUN))
		{
			result += "Ammo for all guns.\n";
			//if more ammo than guns, all guns operational
			guns_temp = inventory_.at(ItemType::GUN);
		}
		else
		{
			char ammo_for[50];
			//else if we have say 5 bullets and 10 guns, there are really only 5 guns available
			sprintf(ammo_for, "Ammo for %d guns\n", inventory_.at(ItemType::AMMO));
			result += ammo_for;
			guns_temp = inventory_.at(ItemType::AMMO);
		}
		if (guns_temp >= hands_)
		{
			//all hands have guns, 
			result += "Guns for all hands + (" + std::to_string(hands_ * 10) + ")\n";
			strength += hands_ * 10;
		}
		else if (hands_ > guns_temp)
		{
			result += "Guns for " + std::to_string(guns_temp) + " hands + (" + std::to_string(guns_temp * 10) + ")\n";
			//if not enough guns to go around
			//add 10 for every person with a gun
			int temp = hands_;
			strength += guns_temp * 10;
			temp -= guns_temp;
			//add 2 for the rest without guns
			strength += temp * 2;
			result += std::to_string(temp) + " unarmed hands + (" + std::to_string(temp * 2) + ")\n";
		}
		return strength;
	}




}