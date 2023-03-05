#pragma once

#include <Constants.h>
#include <filesystem>
#include <ECS/Components/Components.hpp>
#include <nlohmann/json.hpp>


namespace SXNGN {
	namespace ECS {
			class JSON_Utils 
			{
			public:
				static std::tuple<std::vector<Pre_Renderable*>, std::vector<Collisionable*>, std::vector<Location*>, std::vector<Tile*>>
					json_to_tile_batch(nlohmann::json);
				
				static nlohmann::json component_to_json(const ECS_Component* component);
				static ECS_Component* json_to_component(nlohmann::json json);
			};

		}
	}



	
