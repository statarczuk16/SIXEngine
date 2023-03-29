#include <ECS/Core/Item.hpp>
#include <nlohmann/json.hpp>
using nlohmann::json;

namespace SXNGN::ECS {

	ItemType Item::get_type()
	{
		return type_;
	}



	

}