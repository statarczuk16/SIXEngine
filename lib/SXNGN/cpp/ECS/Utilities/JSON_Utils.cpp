#include <ECS/Utilities/JSON_Utils.hpp>
#include <ECS/Utilities/Entity_Builder_Utils.hpp>
#include <iostream>


namespace SXNGN {
	namespace ECS {
		namespace A {
			std::tuple<std::vector<A::Pre_Renderable>, std::vector<A::Collisionable>, std::vector<A::Tile>>
				JSON_Utils::json_to_tile_batch(nlohmann::json jf)
			{
				int tiles_wide = 0;
				int tiles_high = 0;
				std::string default_tile = SXNGN::BAD_STRING_RETURN;
				std::string default_tileset = SXNGN::BAD_STRING_RETURN;
				printf("Reading tile_batch from JSON\n");
				for (auto& [key, val] : jf.items())
				{
					if (key == "tiles_width")
					{
						tiles_wide = val.get<int>();
					}
					else if (key == "tiles_height")
					{
						tiles_high = val.get<int>();
					}
					else if (key == "default_tileset")
					{
						default_tileset = val.get<std::string>();
					}
					else if (key == "default_tile")
					{
						default_tile = val.get<std::string>();
					}
					else
					{
						printf("!!!JSON_Utils::Error reading tile_batch with: \n");
						std::cout << "key: " << key << ", value:" << val << '\n';
						printf("!!! \n");
					}
				}

				printf("Preparing tile batch with:\n");
				printf("Tileset: %s\n", default_tileset.c_str());
				printf("Base tile: %s\n", default_tile.c_str());
				printf("Width: %d\n", tiles_wide);
				printf("Height: %d\n", tiles_high);

				std::vector<A::Pre_Renderable> pre_renders;
				std::vector<A::Collisionable> collisionables;
				std::vector<A::Tile> tiles;

				for (int h = 0; h < tiles_high; h++)
				{
					for (int w = 0; w < tiles_wide; w++)
					{
						Sint32 x_pixels = w * SXNGN::BASE_TILE_WIDTH;
						Sint32 y_pixels = h * SXNGN::BASE_TILE_HEIGHT;
						SDL_Rect collision_box;
						collision_box.x = x_pixels;
						collision_box.y = y_pixels;
						collision_box.w = SXNGN::BASE_TILE_WIDTH;
						collision_box.h = SXNGN::BASE_TILE_HEIGHT;
						auto pre_render = new A::Pre_Renderable(x_pixels, y_pixels, default_tileset, default_tile, A::RenderLayer::GROUND_LAYER);
						auto collision = Entity_Builder_Utils::Create_Collisionable(collision_box, A::CollisionType::NONE);
						auto tile = Entity_Builder_Utils::Create_Tile(w, h);
						pre_renders.push_back(*pre_render);
						collisionables.push_back(*collision);
						tiles.push_back(*tile);
					}
				}
				return std::make_tuple(pre_renders, collisionables, tiles);
			}

			/// <summary>
			/// Take in a pointer to a component, return JSON conversion
			/// </summary>
			/// <param name="component_ptr"></param>
			/// <returns></returns>
			nlohmann::json JSON_Utils::component_to_json(const ECS_Component* component_ptr)
			{
				json js;
				switch (component_ptr->component_type)
				{
				case ComponentTypeEnum::RENDERABLE :
				{
					Renderable component = *static_cast<const Renderable*>(component_ptr);
					js = component;
					return js;
					break;
				}
				case ComponentTypeEnum::PRE_RENDERABLE:
				{
					Pre_Renderable component = *static_cast<const Pre_Renderable*>(component_ptr);
					js = component;
					return js;
					break;
				}
				default :
				{
					std::string component_type_str = "Component type_ not in component_type_enum_to_string";
					if (component_type_enum_to_string().count(component_ptr->component_type))
					{
						component_type_str = component_type_enum_to_string().at(component_ptr->component_type);
					}

					printf("JSON_Utils::component_to_json No conversion to JSON available for type_ %s\n", component_type_str.c_str());
					js["component_type_"] = component_type_enum_to_string().at(component_ptr->component_type) + "_NO_JSON_CONVERSION";
					return js;
				}
				}
			}

			ECS_Component* JSON_Utils::json_to_component(nlohmann::json json)
			{
				auto component_type = json.find("component_type");
				if (component_type == json.end())
				{
					return nullptr;
				}
				std::string component_type_str = *component_type;
				 auto find_type = ECS::A::component_type_string_to_enum().find(component_type_str);
				 if (find_type == ECS::A::component_type_string_to_enum().end())
				 {
					 return nullptr;
				 }
				ComponentTypeEnum component_type_enum = find_type->second;
				switch (component_type_enum)
				{
					case ComponentTypeEnum::JSON_ENTITY:
					{
						auto component_inst = json.get<ExternJSONEntity>();
						ECS_Component* component_ptr = new ExternJSONEntity(component_inst);
						return component_ptr;
						break;
					}
					case ComponentTypeEnum::PRE_RENDERABLE:
					{
						auto component_inst = json.get<Pre_Renderable>();
						ECS_Component* component_ptr = new Pre_Renderable(component_inst);
						return component_ptr;
						break;
					}
				}
			}
		}
	}
}