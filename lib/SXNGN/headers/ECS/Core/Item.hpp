#pragma once
#include <SDL.h>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <ECS/Core/Types.hpp>

using nlohmann::json;

namespace SXNGN::ECS {

	class UIContainerComponent;

    enum ItemType : Uint32 {
        UNKNOWN,
        FOOD,
		FOOTWEAR,
        WATER,
        MEDKIT,
        GUN,
        AMMO,
        BATTERY,
        GPS,
		KALNOTE,
		END
    };



	//convert enum to string for json purposes
	inline std::unordered_map<ItemType, std::string>& item_type_to_string() {
		static std::unordered_map<ItemType, std::string> item_type_to_string =
		{
			{ItemType::UNKNOWN,"UNKNOWN"},
			{ItemType::FOOD,"FOOD"},
			{ItemType::FOOTWEAR,"BOOTS"},
			{ItemType::WATER,"WATER"},
			{ItemType::MEDKIT,"MEDKIT"},
			{ItemType::GUN,"GUN"},
			{ItemType::AMMO,"AMMO"},
			{ItemType::BATTERY,"BATTERY"},
			{ItemType::KALNOTE,"KALNOTE"},
			{ItemType::GPS,"GPS"}
		};
		return item_type_to_string;
	}

	//convert string to enum for JSON purposes
	inline std::unordered_map<std::string, ItemType>& string_to_item_type() {
		static std::unordered_map<std::string, ItemType> string_to_item_type =
		{
			{"UNKNOWN",ItemType::UNKNOWN},
			{"FOOD",ItemType::FOOD},
			{"FOOTWEAR",ItemType::FOOTWEAR},
			{"WATER",ItemType::WATER},
			{"MEDKIT",ItemType::MEDKIT},
			{"GUN",ItemType::GUN},
			{"AMMO",ItemType::AMMO},
			{"BATTERY",ItemType::BATTERY},
			{"KALNOTE",ItemType::KALNOTE},
			{"FOOD",ItemType::FOOD}
		};
		return string_to_item_type;
	}

	//convert enum to string for json purposes
	inline std::unordered_map<ItemType, double>& item_type_to_weight_kg() {
		static std::unordered_map<ItemType, double> item_type_to_weight_kg =
		{
			{ItemType::UNKNOWN, 0.0},
			{ItemType::FOOD, 0.01}, //1 unit = 10 calories, so like half a quarter of a granola bar
			{ItemType::KALNOTE, 0.001}, //1 unit = 10 calories, so like half a quarter of a granola bar
			{ItemType::FOOTWEAR,2.0},
			{ItemType::WATER,1}, //0.5 liter = 0.5 kg
			{ItemType::MEDKIT,10},
			{ItemType::GUN,1},
			{ItemType::AMMO,0.01},
			{ItemType::BATTERY,0.15},
			{ItemType::GPS,1}
		};
		return item_type_to_weight_kg;
	}

	//base rarity of items
	inline std::unordered_map<ItemType, int>& item_type_to_table_weight() {
		static std::unordered_map<ItemType, int> item_type_to_table_weight =
		{
			{ItemType::UNKNOWN, 0.0},
			{ItemType::FOOD, 100}, //1 unit = 10 calories, so like half a quarter of a granola bar
			{ItemType::KALNOTE, 15}, //1 unit = 10 calories, so like half a quarter of a granola bar
			{ItemType::FOOTWEAR, 15},
			{ItemType::WATER, 0}, //0.5 liter = 0.5 kg
			{ItemType::MEDKIT, 10},
			{ItemType::GUN, 5},
			{ItemType::AMMO, 30},
			{ItemType::BATTERY, 12},
			{ItemType::GPS, 5}
		};
		return item_type_to_table_weight;
	}

	//base accumulation of items
	inline std::unordered_map<ItemType, double>& item_type_to_table_acc() {
		static std::unordered_map<ItemType, double> item_type_to_table_acc =
		{
			{ItemType::UNKNOWN, 0.0},
			{ItemType::FOOD, 100}, //1 unit = 10 calories, so like half a quarter of a granola bar
			{ItemType::KALNOTE, 15}, //1 unit = 10 calories, so like half a quarter of a granola bar
			{ItemType::FOOTWEAR, 15},
			{ItemType::WATER, 0}, //0.5 liter = 0.5 kg
			{ItemType::MEDKIT, 10},
			{ItemType::GUN, 5},
			{ItemType::AMMO, 30},
			{ItemType::BATTERY, 12},
			{ItemType::GPS, 5}
		};
		return item_type_to_table_acc;
	}

	//convert enum to string for json purposes
	inline std::unordered_map<ItemType, double>& item_type_to_base_value_kl() {
		static std::unordered_map<ItemType, double> item_type_to_base_value_kl =
		{
			{ItemType::UNKNOWN, 0.0},
			{ItemType::FOOD, 0.015}, //1 unit = 10 calories, 1 kalnote buys 700 calories
			{ItemType::KALNOTE, 1}, //this is the currency
			{ItemType::FOOTWEAR, 10.0},
			{ItemType::WATER,1}, //0.5 liter = 0.5 kg
			{ItemType::MEDKIT,15.0},
			{ItemType::GUN,25.0},
			{ItemType::AMMO,0.05},
			{ItemType::BATTERY,2},
			{ItemType::GPS,100}
		};
		return item_type_to_base_value_kl;
	}

	class Item
	{

	public:
		Item(ItemType type = ItemType::UNKNOWN,
			int rarity = 10,
			int acc = 1,
			int dec = 5,
			int max_rarity = 100,
			int min_rarity = 0,
			double base_value_kn = 1,
			double weight_kg = 1) :
			type_(type),
			name_(item_type_to_string()[type]),
			rarity_(rarity),
			acc_(acc),
			dec_(dec),
			max_rarity_(max_rarity),
			min_rarity_(min_rarity),
			base_value_kn_(base_value_kn),
			weight_kg_(weight_kg)
		{

		}

		ItemType get_type();
		ItemType type_;
		std::string name_;
		//loot table stuff
		int rarity_; //base weight in loot tables (higher is less rare)
		int acc_; //base accumulation loot tables (weight increases this much if NOT chosen for drop)
		int dec_; //base penalty in loot tables (weight decreases this much if chosen for drop)
		int max_rarity_; //max weight for loot drop
		int min_rarity_; //minimum weight for loot drop
		//gameplay stuff
		double base_value_kn_; //base value in kalnotes (kn) 
		double weight_kg_;
	};

	inline void to_json(json& j, const Item& p) {
		ItemType type_ = p.type_;
		j = json{
			{"type_",item_type_to_string()[type_]},
		};

	}

	inline void from_json(const json& j, Item& p) {
		auto item_type = string_to_item_type().at(j.at("type_"));
		p.type_ = item_type;
	}

	//convert enum to string for json purposes
	inline std::unordered_map<ItemType, Item>& item_type_to_item() {
		static std::unordered_map<ItemType, Item> item_type_to_item =
		{
			{ItemType::UNKNOWN, Item()},
			{ItemType::FOOD, Item()},
			{ItemType::KALNOTE, Item()}, 
			{ItemType::FOOTWEAR, Item()},
			{ItemType::WATER, Item()},
			{ItemType::MEDKIT, Item()},
			{ItemType::GUN, Item()},
			{ItemType::AMMO, Item()},
			{ItemType::BATTERY, Item()},
			{ItemType::GPS, Item()}
		};
		return item_type_to_item;
	}

	class TradeHelper
	{
		

		public:
			
			Entity left_trader = -1;
			Entity right_trader = -1;
			double left_running_total = 0;
			double right_running_total = 0;
			bool left_is_player = true;
			std::map<ItemType, double> left_inv;
			std::map<ItemType, double> right_inv;
			std::map<ItemType, double> left_inv_temp;
			std::map<ItemType, double> right_inv_temp;
			std::map<ItemType, double> item_values;
			std::shared_ptr<UIContainerComponent> left_side_total_label = nullptr;
			std::shared_ptr<UIContainerComponent> right_side_total_label = nullptr;
			std::shared_ptr<UIContainerComponent> confirm_button = nullptr;


	};

	

}