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
		SCRAP,
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
			{ItemType::GPS,"GPS"},
			{ItemType::SCRAP,"SCRAP"}
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
			{"FOOD",ItemType::FOOD},
			{"SCRAP",ItemType::SCRAP}
		};
		return string_to_item_type;
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
			max_find_weight_(max_rarity),
			min_find_weight_(min_rarity),
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
		int max_find_weight_; //max weight for loot drop
		int min_find_weight_; //minimum weight for loot drop
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
		static std::unordered_map<ItemType, Item> item_type_to_item;
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