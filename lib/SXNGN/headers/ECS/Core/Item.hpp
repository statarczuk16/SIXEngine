#pragma once
#include <SDL.h>
#include <unordered_map>
#include <nlohmann/json.hpp>
using nlohmann::json;

namespace SXNGN::ECS {


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
    
	class Item
	{
		Item();

	public:

		ItemType get_type();
		ItemType type_;

	private:

		double weight_;
		
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

	class TradeHelper
	{
		

		public:
			TradeHelper();
			std::map<ItemType, double> left_inv;
			std::map<ItemType, double> right_inv;
			std::map<ItemType, double> left_inv_temp;
			std::map<ItemType, double> right_inv_temp;
			std::map<ItemType, double> item_values;

	};

	

}