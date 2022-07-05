#include <ECS/Utilities/JSON_Utils.hpp>
#include <ECS/Utilities/Entity_Builder_Utils.hpp>
#include <iostream>
#include <ECS/Utilities/Map_Utils.hpp>



namespace SXNGN {
	namespace ECS {
		namespace A {
			std::tuple<std::vector<Pre_Renderable*>, std::vector<Collisionable*>, std::vector<Location*>>
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

				

				auto tile_map = Map_Utils::CreateTileMap(tiles_wide * TILES_TO_CHUNK_EDGE, tiles_high * TILES_TO_CHUNK_EDGE, default_tileset, default_tile);

				return tile_map;
			}

			/// <summary>
			/// Take in a pointer to a component, return JSON conversion
			/// </summary>
			/// <param name="component_ptr"></param>
			/// <returns></returns>
			nlohmann::json JSON_Utils::component_to_json(const ECS_Component* component_ptr)
			{
				json js;

				if (component_ptr->component_type > ComponentTypeEnum::GAME_STATE_START_HERE)
				{
					ECS_Component component = *static_cast<const ECS_Component*>(component_ptr);
					js = component;
					return js;
				}

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
				case ComponentTypeEnum::COLLISION:
				{
					Collisionable component = *static_cast<const Collisionable*>(component_ptr);
					js = component;
					return js;
					break;
				}
				case ComponentTypeEnum::TILE:
				{
					Tile component = *static_cast<const Tile*>(component_ptr);
					js = component;
					return js;
					break;
				}
				case ComponentTypeEnum::SELECTABLE:
				{
					Selectable component = *static_cast<const Selectable*>(component_ptr);
					js = component;
					return js;
					break;
				}
				case ComponentTypeEnum::INPUT_TAGS:
				{
					User_Input_Tags_Collection component = *static_cast<const User_Input_Tags_Collection*>(component_ptr);
					js = component;
					return js;
					break;
				}
				case ComponentTypeEnum::MOVEABLE:
				{
					Moveable component = *static_cast<const Moveable*>(component_ptr);
					js = component;
					return js;
					break;
				}
				case ComponentTypeEnum::TASK_WORKER:
				{
					Task_Worker_Component component = *static_cast<const Task_Worker_Component*>(component_ptr);
					js = component;
					return js;
					break;
				}
				case ComponentTypeEnum::TASK:
				{
					Task_Component component = *static_cast<const Task_Component*>(component_ptr);
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

			ECS_Component* JSON_Utils::json_to_component(nlohmann::json j)
			{
				auto component_type = j.find("component_type");
				if (component_type == j.end())
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
				if (component_type_enum > ComponentTypeEnum::GAME_STATE_START_HERE)
				{
					ECS_Component* component = Create_Gamestate_Component_from_Enum(component_type_enum);
					return component;
				}
				switch (component_type_enum)
				{
					case ComponentTypeEnum::JSON_ENTITY:
					{
						auto component_inst = j.get<ExternJSONEntity>();
						ECS_Component* component_ptr = new ExternJSONEntity(component_inst);
						return component_ptr;
						break;
					}
					case ComponentTypeEnum::PRE_RENDERABLE:
					{
						auto component_inst = j.get<Pre_Renderable>();
						ECS_Component* component_ptr = new Pre_Renderable(component_inst);
						return component_ptr;
						break;
					}
					case ComponentTypeEnum::COLLISION:
					{
						auto component_inst = j.get<Collisionable>();
						ECS_Component* component_ptr = new Collisionable(component_inst);
						return component_ptr;
						break;
					}
					case ComponentTypeEnum::TILE:
					{
						auto component_inst = j.get<Tile>();
						ECS_Component* component_ptr = new Tile(component_inst);
						return component_ptr;
						break;
					}
					case ComponentTypeEnum::SELECTABLE:
					{
						auto component_inst = j.get<Selectable>();
						ECS_Component* component_ptr = new Selectable(component_inst);
						return component_ptr;
						break;
					}
					case ComponentTypeEnum::INPUT_TAGS:
					{
						auto component_inst = j.get<User_Input_Tags_Collection>();
						ECS_Component* component_ptr = new User_Input_Tags_Collection(component_inst);
						return component_ptr;
						break;
					}
					case ComponentTypeEnum::MOVEABLE:
					{
						auto component_inst = j.get<Moveable>();
						ECS_Component* component_ptr = new Moveable(component_inst);
						return component_ptr;
						break;
					}
					case ComponentTypeEnum::TASK_WORKER:
					{
						auto component_inst = j.get<Task_Worker_Component>();
						ECS_Component* component_ptr = new Task_Worker_Component(component_inst);
						return component_ptr;
						break;
					}
					case ComponentTypeEnum::TASK:
					{
						auto component_inst = j.get<Task_Component>();
						ECS_Component* component_ptr = new Task_Component(component_inst);
						return component_ptr;
						break;
					}
					default:
					{
						SDL_LogWarn(1, "JSON_Utils::json_to_component: type: %s is not implemented.", find_type);
						return nullptr;
					}
				}
			}
		}
	}
}