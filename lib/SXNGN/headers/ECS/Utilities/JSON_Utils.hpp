#pragma once

#include <Constants.h>
#include <filesystem>
#include <ECS/Components/Components.hpp>
#include <nlohmann/json.hpp>


namespace SXNGN {
	namespace ECS {
		namespace A {
			class JSON_Utils 
			{
			public:
				static std::tuple<std::vector<Pre_Renderable*>, std::vector<Collisionable*>, std::vector<Location*>>
					json_to_tile_batch(nlohmann::json);
				
				static nlohmann::json component_to_json(const ECS_Component* component);
				static ECS_Component* json_to_component(nlohmann::json json);
			};



			/**
			void to_json(json& j, const SDL_Rect& p) {
				j = json{ {"x", p.x}, {"y", p.y}, {"w", p.w}, {"h", p.h} };
			}

			void from_json(const json& j, SDL_Rect& p) {
				j.at("x").get_to(p.x);
				j.at("y").get_to(p.y);
				j.at("w").get_to(p.w);
				j.at("h").get_to(p.h);
			}
			**/
		}
	}
}



	
