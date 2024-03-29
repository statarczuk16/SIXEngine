#pragma once

#include "ECS/Core/ComponentManager.hpp"
#include "ECS/Core/EntityManager.hpp"
#include "ECS/Core/EventManager.hpp"
#include "ECS/Core/Types.hpp"
#include <memory>
#include <ECS/Core/Component.hpp>
#include <ECS/Core/TextureManager.hpp>
#include <ECS/Components/Components.hpp>
#include <ECS/Utilities/JSON_Utils.hpp>
#include <ECS/Core/StateManager.hpp>
#include <ECS/Core/SystemManager.hpp>
#include <iostream>
#include <array>



namespace SXNGN {
	namespace ECS {
			using ECS_Component = ECS_Component;


			class Coordinator
			{
			public:
				void Init(SDL_Renderer* renderer)
				{
					mComponentManager = std::make_shared<ComponentManager>();
					mEntityManager = std::make_shared<EntityManager>();
					mEventManager = std::make_shared<EventManager>();
					mSystemManager = std::make_shared<SystemManager>();
					mTextureManager = std::make_shared<TextureManager>(renderer);
				}

				void InitStateManager()
				{
					mStateManager = std::make_shared<StateManager>();
				}


				// Entity methods
				Entity CreateEntity(sole::uuid uuid = SXNGN::BAD_UUID, bool quiet = false)
				{
					return mEntityManager->CreateEntity(uuid, quiet);
				}

				Entity GetEntityFromUUID(sole::uuid uuid)
				{
					return mEntityManager->GetEntityFromUUID(uuid);
				}

				sole::uuid GetUUIDFromEntity(Entity entity)
				{
					return mEntityManager->GetUUIDFromEntity(entity);
				}

				void DestroyEntity(Entity entity)
				{

					mSystemManager->EntityDestroyed(entity);

					sole::uuid uuid = GetUUIDFromEntity(entity);
					mStateManager->removeUUIDFromLocationMap(uuid);
					updateCollisionMap(uuid, SXNGN::DEFAULT_SPACE);
					mEntityManager->DestroyEntity(entity);

					//takes the longest because of multithread safety so do last
					//no one will try to act on the data after system manager thinks it is gone, I think...
					mComponentManager->EntityDestroyed(entity);
					
				}

				/// <summary>
				/// Check if an entity has a component
				/// </summary>
				/// <param name="entity"></param>
				/// <param name="component_type"></param>
				/// <returns></returns>
				bool EntityHasComponent(Entity entity, ComponentTypeEnum component_type)
				{
					//The hash is a Uint8 that increments for each component type added - makes sequential storage and signatures easier. Not the same as the underlying enum value.
					if (!mEntityManager->EntityLiving(entity))
					{
						return false;
					}
					auto component_type_hash = mComponentManager->GetComponentType(component_type);
					auto entity_signature = mEntityManager->GetSignature(entity);
					return entity_signature[component_type_hash];
				}

				uint32_t GetLivingEntityCount()
				{
					return mEntityManager->GetLivingEntityCount();
				}


				// Component methods

				void RegisterComponent(ComponentTypeEnum component_type)
				{
					mComponentManager->RegisterComponent(component_type);
				}

				void AddComponent(Entity entity, ECS_Component* component, bool quiet = true)
				{
					if (!quiet)
					{
						SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Entity Update %d : New Component: %d", entity, component->get_component_type());
					}

					mComponentManager->AddComponent(entity, component);

					auto signature = mEntityManager->GetSignature(entity);
					signature.set(mComponentManager->GetComponentType(component->get_component_type()), true);
					mEntityManager->SetSignature(entity, signature);

					mSystemManager->EntitySignatureChanged(entity, signature, quiet);
					auto game_states = GetActiveGameStates();
					if (game_states.count(ComponentTypeEnum::TACTICAL_STATE))
					{
						if (component->get_component_type() == ComponentTypeEnum::LOCATION)
						{
							Location* location_ptr = static_cast<Location*>(component);
							if (location_ptr->m_track_in_grid_map_ == true)
							{
								Coordinate grid = location_ptr->GetGridCoordinate();
								int grid_x = grid.x;
								int grid_y = grid.y;
								sole::uuid uuid = mEntityManager->GetUUIDFromEntity(entity);
								addUUIDToLocationMap(grid_x, grid_y, uuid, SXNGN::DEFAULT_SPACE);
								updateCollisionMap(grid_x, grid_y, SXNGN::DEFAULT_SPACE);
							}
						}
					}
					
				}

				
				void RemoveComponent(Entity entity, ComponentTypeEnum component_type)
				{
					mComponentManager->RemoveComponent(entity, component_type);

					auto signature = mEntityManager->GetSignature(entity);
					signature.set(mComponentManager->GetComponentType(component_type), false);
					mEntityManager->SetSignature(entity, signature);

					mSystemManager->EntitySignatureChanged(entity, signature);
				}

				/// <summary>
				/// Change all existing systems to require an entity to contain one of the active_game_states Components in order to be seen by the system
				/// (This is how to change game state)
				/// </summary>
				/// <param name="active_game_states"></param>
				/// <param name="quiet"></param>
				void GameStateChanged(std::set<ComponentTypeEnum> active_game_states, bool quiet = true)
				{
					printf("Changing game state\n");
					mStateManager->gameStateChanged(active_game_states);
					active_game_states = mStateManager->getActiveGameStates();
					Signature game_state_signature;
					for (auto state : active_game_states)
					{
						game_state_signature.set(GetComponentType(state));
					}

					auto entity_signatures = mEntityManager->GetAllEntitySignatures();

					mSystemManager->GameStateChanged(game_state_signature, entity_signatures, quiet);
				}

				std::set<ComponentTypeEnum> GetActiveGameStates()
				{
					return mStateManager->getActiveGameStates();
				}

				void addUUIDToLocationMap(int grid_x, int grid_y, sole::uuid uuid, std::string space = SXNGN::DEFAULT_SPACE)
				{
					mStateManager->addUUIDToLocationMap(grid_x, grid_y, uuid, space);
					updateCollisionMap(uuid, space);
				}

				int removeUUIDFromLocationMap(sole::uuid uuid, std::string space = SXNGN::DEFAULT_SPACE)
				{
					int ret = mStateManager->removeUUIDFromLocationMap(uuid, space);
					return ret;
				}

				int moveUUIDOnLocationMap(int grid_x_from, int grid_y_from, int grid_x_to, int grid_y_to, sole::uuid uuid, std::string space = SXNGN::DEFAULT_SPACE)
				{
					return mStateManager->moveUUIDOnLocationMap(grid_x_from, grid_y_from, grid_x_to, grid_y_to, uuid, space);
				}

				std::pair<double, bool> getSetting(std::string setting)
				{
					auto settings_component = DatabaseComponent::get_instance();
					if (settings_component->settings_map.count(setting) == 0)
					{
						return std::make_pair(0.0, false);
					}
					else
					{
						return std::make_pair(settings_component->settings_map.at(setting), true);
					}
				}
				sole::uuid getUUID(std::string entity)
				{
					auto settings_component = DatabaseComponent::get_instance();
					if (settings_component->entity_map.count(entity) == 0)
					{
						return SXNGN::BAD_UUID;
					}
					else
					{
						return settings_component->entity_map.at(entity);
					}
				}
				std::pair<Event_Component, bool> getEvent(std::string setting)
				{
					auto settings_component = DatabaseComponent::get_instance();
					
					if (settings_component->event_map.count(setting) == 0)
					{

						return std::make_pair(Event_Component(), false);
					}
					else
					{
						return std::make_pair(settings_component->event_map.at(setting), true);
					}
				}

				void setSetting(std::string setting, double value)
				{
					auto settings_component = DatabaseComponent::get_instance();
					settings_component->settings_map[setting] = value;
				}
				void setUUID(std::string setting, sole::uuid uuid)
				{
					auto settings_component = DatabaseComponent::get_instance();
					settings_component->entity_map[setting] = uuid;
				}
				void setEvent(std::string setting, Event_Component event_component)
				{
					auto settings_component = DatabaseComponent::get_instance();
					settings_component->event_map[setting] = event_component;
				}

				const GameSettings* getGameSettings()
				{
					return mStateManager->getGameSettings();
				}

				int updateCollisionMap(int grid_x, int grid_y, std::string space = SXNGN::DEFAULT_SPACE)
				{
					//get all the other uuids at this location
					std::set<sole::uuid> uuids_at_x_y = mStateManager->getUUIDSAtLocation(grid_x, grid_y, space);

					//set up a collision map if it's not already there for the space
					if (mStateManager->space_to_collision_map_.count(SXNGN::DEFAULT_SPACE) < 1)
					{
						std::vector<std::vector<int > > collision_map;
						for (Uint32 i = 0; i < mStateManager->getGameSettings()->level_settings.level_width_tiles; i++)
						{
							collision_map.push_back(std::vector< int >());
							for (Uint32 j = 0; j < mStateManager->getGameSettings()->level_settings.level_height_tiles; j++)
							{
								collision_map[i].push_back(1);
							}
						}
						mStateManager->space_to_collision_map_[SXNGN::DEFAULT_SPACE] = collision_map;
					}

					//get the collision map for this space
					auto collision_map = mStateManager->space_to_collision_map_.at(SXNGN::DEFAULT_SPACE);
					//get all the collisonable objects
					std::array<ECS_Component*, MAX_ENTITIES>& renderables = CheckOutAllData(ComponentTypeEnum::RENDERABLE);
					std::array<ECS_Component*, MAX_ENTITIES>& collisionables = CheckOutAllData(ComponentTypeEnum::COLLISION);
					std::array<ECS_Component*, MAX_ENTITIES>& tiles = CheckOutAllData(ComponentTypeEnum::TILE);
					int sum_traversal_cost = 0;
					bool traversible = true;
					//go through all the uuids at this x,y and determine the sum collision value
					for (sole::uuid uuid : uuids_at_x_y)
					{
						Entity entity = 0;
						entity = GetEntityFromUUID(uuid);
						if (entity > MAX_ENTITIES || entity == -1)
						{
							removeUUIDFromLocationMap(uuid);
							continue;
						}
						if (collisionables[entity] != nullptr)
						{
							auto collisionable = collisionables[entity];
							auto collision_ptr = static_cast<Collisionable*>(collisionable);
							collision_ptr->resolved_ = false; //if updating collision map, set collision to 
							sum_traversal_cost += collision_ptr->traversal_cost_;
							if (collision_ptr->traversal_cost_ == -1)
							{
								traversible = false;
							}
						}
						if (tiles[entity] != nullptr)
						{
							auto tile_ptr = static_cast<Tile*>(tiles[entity]);
							sum_traversal_cost += tile_ptr->traversal_cost_;
							if (tile_ptr->traversal_cost_ == -1)
							{
								traversible = false;
							}
						}
						if (renderables[entity] != nullptr)
						{
							auto renderable = renderables[entity];
							auto renderable_ptr = static_cast<Renderable*>(renderable);
							renderable_ptr->display_string_debug_ = std::to_string(sum_traversal_cost);
						}
					}
					//update and store the collision map in the state manager
					if (collision_map.size() < grid_x)
					{
						SDL_LogError(1, "Collision Map Bad Index");
						CheckInAllData(ComponentTypeEnum::COLLISION);
						CheckInAllData(ComponentTypeEnum::RENDERABLE);
						CheckInAllData(ComponentTypeEnum::TILE);
						return -1;
					}
					if (collision_map[grid_x].size() < grid_y)
					{
						SDL_LogError(1, "Collision Map Bad Index");
						CheckInAllData(ComponentTypeEnum::COLLISION);
						CheckInAllData(ComponentTypeEnum::RENDERABLE);
						CheckInAllData(ComponentTypeEnum::TILE);
						return -1;
					}
					//if either of these, mark as non-traversible
					if (sum_traversal_cost < 0 || traversible == false)
					{
						traversible = false;
						sum_traversal_cost = -1;
					}
					collision_map[grid_x][grid_y] = sum_traversal_cost;
					mStateManager->space_to_collision_map_[SXNGN::DEFAULT_SPACE] = collision_map;
					CheckInAllData(ComponentTypeEnum::COLLISION);
					CheckInAllData(ComponentTypeEnum::RENDERABLE);
					CheckInAllData(ComponentTypeEnum::TILE);
					return 0;
				}

				int updateCollisionMap(sole::uuid uuid, std::string space = SXNGN::DEFAULT_SPACE)
				{
					//get the x,y grid of this uuid if any
					auto x_y = mStateManager->getLocationOfUUID(uuid, space);
					if (x_y.first == -1)
					{
						return -1;
					}
					return updateCollisionMap(x_y.first, x_y.second, space);
				}

				/// <summary>
				/// Update the entire collision map in this space
				/// </summary>
				/// <param name="space"></param>
				/// <returns></returns>
				int updateCollisionMap(std::string space = SXNGN::DEFAULT_SPACE)
				{
					//set up a collision map if it's not already there for the space
					if (mStateManager->space_to_collision_map_.count(SXNGN::DEFAULT_SPACE) < 1)
					{
						std::vector<std::vector<int > > collision_map;
						for (Uint32 i = 0; i < mStateManager->getGameSettings()->level_settings.level_width_tiles; i++)
						{
							collision_map.push_back(std::vector< int >());
							for (Uint32 j = 0; j < mStateManager->getGameSettings()->level_settings.level_height_tiles; j++)
							{
								collision_map[i].push_back(1);
							}
						}
						mStateManager->space_to_collision_map_[SXNGN::DEFAULT_SPACE] = collision_map;
						return 0;
					}

					int width = mStateManager->getGameSettings()->level_settings.level_width_tiles;
					int height = mStateManager->getGameSettings()->level_settings.level_height_tiles;
					for (Uint32 i = 0; i < mStateManager->getGameSettings()->level_settings.level_width_tiles; i++)
					{
						for (Uint32 j = 0; j < mStateManager->getGameSettings()->level_settings.level_height_tiles; j++)
						{
							updateCollisionMap(i, j, space);
						}
					}

				}

				std::unordered_map < std::string, std::vector < std::vector < int > > > getCollisionMap()
				{
					return mStateManager->space_to_collision_map_;
				}

				const std::unordered_map < std::string, std::vector < std::vector < std::set < sole::uuid > > > >& getSpaceToEntityMap()
				{
					return mStateManager->getSpaceToEntityMap();
				}



				/// <summary>
				/// Thread safe - thread waits until data is available, then returns with a COPY of it.
				/// </summary>
				/// <param name="entity"></param>
				/// <param name="component_type"></param>
				/// <returns></returns>
				/// 
				///**
				const ECS_Component* GetComponentReadOnly(Entity entity, ComponentTypeEnum component_type)
				{

					return mComponentManager->GetComponentReadOnly(entity, component_type);
				}

				/// <summary>
				/// Thread safe access to data. Thread asks for component of an entity and is given the data 
				/// Do not ever use without calling CheckInComponent when done.
				/// </summary>
				/// <param name="entity"></param>
				/// <param name="component_type"></param>
				/// <returns></returns>
				ECS_Component* CheckOutComponent(Entity entity, ComponentTypeEnum component_type)
				{
					return mComponentManager->CheckOutComponent(entity, component_type);
				}

				const std::array<ECS_Component*, MAX_ENTITIES> GetComponentArrayReadOnly(ComponentTypeEnum component_type)
				{
					return mComponentManager->GetComponentArrayReadOnly(component_type);
				}

				std::array<ECS_Component*, MAX_ENTITIES>& CheckOutAllData(ComponentTypeEnum component_type)
				{
					return mComponentManager->CheckOutAllData(component_type);
				}

				void CheckInAllData(ComponentTypeEnum component_type)
				{
					mComponentManager->CheckInAllData(component_type);
				}

				std::array<ECS_Component*, MAX_ENTITIES> GetComponentArray(ComponentTypeEnum component_type)
				{
					return mComponentManager->GetComponentArrayReadOnly(component_type);
				}

				/// <summary>
				/// Used by a thread to return checked-out data.
				/// </summary>
				/// <param name="entity"></param>
				/// <param name="component_type"></param>
				/// <param name="data_and_key"></param>
				void CheckInComponent(ComponentTypeEnum component_type, Entity entity)
				{
					return mComponentManager->CheckInComponent(component_type, entity);
				}

				void CheckInComponent(Entity entity, ComponentTypeEnum component_type)
				{
					return mComponentManager->CheckInComponent(component_type, entity);
				}

				ComponentTypeHash GetComponentType(ComponentTypeEnum component_type)
				{
					return mComponentManager->GetComponentType(component_type);
					//return ComponentTypeHash(component_type);
				}

				template<typename T>
				// A methods
				std::shared_ptr<T> RegisterSystem()
				{
					return mSystemManager->RegisterSystem<T>();
				}

				template<typename T>
				void SetSystemSignatureActable(Signature signature)
				{
					mSystemManager->SetSignatureActable<T>(signature);
				}

				template<typename T>
				void SetSystemSignatureOfInterest(Signature signature)
				{
					mSystemManager->SetSignatureOfInterest<T>(signature);
				}

				

				// Event methods
				void AddEventListener(EventId eventId, std::function<void(Event&)> const& listener)
				{
					mEventManager->AddListener(eventId, listener);
				}

				void SendEvent(Event& event)
				{
					mEventManager->SendEvent(event);
				}

				void SendEvent(EventId eventId)
				{
					mEventManager->SendEvent(eventId);
				}

				std::shared_ptr<TextureManager> get_texture_manager()
				{
					return mTextureManager;
				}

				std::shared_ptr<StateManager> get_state_manager()
				{
					return mStateManager;
				}

				SDL_Renderer* Get_Renderer()
				{
					return mTextureManager->get_renderer();
				}

				/// <summary>
				/// Remove the entity from the system
				/// Return all components that the entity had associated with it
				/// </summary>
				/// <param name="entity"></param>
				/// <returns></returns>
				std::vector<ECS_Component*> Extract_Entity_Components_From_ECS(Entity entity)
				{

					auto extracted_components = mComponentManager->ExtractEntity(entity);
					mSystemManager->EntityDestroyed(entity);
					mEntityManager->DestroyEntity(entity);
					return extracted_components;
				}

				/// <summary>
				/// Remove the entity from the system
				/// Return all components that the entity had associated with it
				/// </summary>
				/// <param name="entity"></param>
				/// <returns></returns>
				std::shared_ptr<ExternEntity> Extract_Entity_From_ECS(Entity entity)
				{
					sole::uuid uuid = GetUUIDFromEntity(entity);
					if (uuid == BAD_UUID)
					{
						SDL_LogError(1, "Entity has no UUID");
						abort();
					}
					auto extracted_components = Extract_Entity_Components_From_ECS(entity);
					auto extracted_entity = std::make_shared<ExternEntity>(entity, uuid, extracted_components);
					return extracted_entity;
				}

				/// <summary>
				/// Remove all entities with input ComponentTypeEnum from ECS and return
				/// </summary>
				/// <param name="component_type"></param>
				std::vector<std::shared_ptr<ExternEntity>> ExtractEntitiesWithMatchingComponent(ComponentTypeEnum component_type)
				{
					std::vector<std::shared_ptr<ExternEntity>> ret;
					std::vector<std::pair<Entity, Signature>> living_entity_sigs = mEntityManager->GetAllEntitySignatures();
					for (auto entity_sig : living_entity_sigs)
					{
						auto component_type_hash = mComponentManager->GetComponentType(component_type);
						if (entity_sig.second[component_type_hash])
						{
							auto extracted_entity = Extract_Entity_From_ECS(entity_sig.first);
							ret.push_back(extracted_entity);
						}
					}

					return ret;
				}

				/// <summary>
				/// Cache an extern_entity into a space - outside of the ECS
				/// </summary>
				/// <param name="entity_to_store"></param>
				/// <param name="state_to_store_in"></param>
				void Space_Extern_Entity(std::shared_ptr<ExternEntity> entity_to_store, std::string state_to_store_in = "Temp")
				{
					mStateManager->cacheEntityInSpace(entity_to_store, state_to_store_in);
				}

				/// <summary>
				/// Take a space full of ExternEntities and dump into the ECS
				/// Optional, destroy the vector after.
				/// </summary>
				/// <param name="state_to_dump_from"></param>
				/// <param name="destroy_each"></param>
				void Dump_Space_To_ECS(std::string state_to_dump_from = "Temp", bool destroy_after = false)
				{
					std::vector< std::shared_ptr<ExternEntity>> entity_array = mStateManager->retrieveSpaceEntities(state_to_dump_from, destroy_after);
					for (auto entity : entity_array)
					{
						Dump_Spaced_Entity_To_ECS(entity);
					}
				}

				/// <summary>
				/// Add an ExternEntity back into the system
				/// </summary>
				/// <param name="entity_to_dump"></param>
				void Dump_Spaced_Entity_To_ECS(std::shared_ptr<ExternEntity> entity_to_dump)
				{
					Entity new_id = mEntityManager->CreateEntity(entity_to_dump->uuid_);
					for (ECS_Component* component : entity_to_dump->entity_components_)
					{
						if (component->component_type == ComponentTypeEnum::DATABASE_SINGLE)
						{
							auto db_shared = DatabaseComponent::init_instance();
							DatabaseComponent* db_comp = static_cast<DatabaseComponent*>(component);
							db_shared->merge_db(db_comp);
						}
						AddComponent(new_id, component);
					}
				}

				/// <summary>
				/// Returns const reference to all components related to an entity
				/// </summary>
				/// <param name="entity"></param>
				/// <returns></returns>
				std::vector<const ECS_Component*> Get_All_Entity_Data_Read_Only(Entity entity)
				{
					return mComponentManager->Get_All_Entity_Data_Read_Only(entity);
				}

				/// <summary>
				/// Returns signatures of all living entities
				/// </summary>
				/// <returns></returns>
				std::vector<std::pair<Entity, Signature>> Get_All_Entity_Signatures()
				{
					return mEntityManager->GetAllEntitySignatures();
				}


				/// <summary>
				/// Copies all components related to an entity and outputs as JSON
				/// </summary>
				/// <param name="entity"></param>
				/// <returns></returns>
				json Entity_To_JSON(Entity entity, sole::uuid uuid)
				{
					std::vector<json> json_components;
					std::vector<const ECS_Component*> components = Get_All_Entity_Data_Read_Only(entity);
					for (auto component_ptr : components)
					{
						json js = JSON_Utils::component_to_json(component_ptr);
						json_components.push_back(js);
					}
					ExternJSONEntity extern_entity(entity, json_components, uuid);
					json ret = extern_entity;
					return ret;
				}

				std::shared_ptr<ExternEntity> JSON_To_Entity(json j)
				{
					std::vector<json> json_components;
					auto component_type = j.find("component_type");
					if (component_type == j.end())
					{
						return nullptr;
					}
					std::string component_type_str = *component_type;
					auto find_type = ECS::component_type_string_to_enum().find(component_type_str);
					if (find_type == ECS::component_type_string_to_enum().end())
					{
						return nullptr;
					}
					//for now only read things marked as JSON_ENTITY, which include a list of components
					ComponentTypeEnum component_type_enum = find_type->second;
					if (component_type_enum != ComponentTypeEnum::JSON_ENTITY)
					{
						SDL_LogError(1, "Cannot Read Incoming JSON because is not JSON_ENTITY: ");
						std::cout << j.dump(4) << std::endl;
						return nullptr;
					}
					//find the list of components
					auto list_of_components = j.find("entity_components_");
					if (list_of_components == j.end())
					{
						return nullptr;
					}
					std::shared_ptr<ExternEntity> extern_entity = std::make_shared<ExternEntity>();
					//convert each one to a C++ object 
					for (auto component_pair : *list_of_components)
					{
						try
						{
							auto component = JSON_Utils::json_to_component(component_pair);
							extern_entity->entity_components_.push_back(component);
						}
						catch (const std::exception& exc)
						{
							std::cerr << "Error converting JSON to Object: " << component_pair.dump(4) << std::endl;
							std::cerr << exc.what() << std::endl;
						}
					}
					auto uuid_it = j.find("uuid_");
					if (uuid_it != j.end())
					{
						std::string uuid_string = *uuid_it;
						sole::uuid uuid = sole::rebuild(uuid_string);
						extern_entity->uuid_ = uuid;
					}
					else
					{
						std::cout << "Incoming JSON Entity had no UUID" << std::endl;
						abort();
					}
					
					return extern_entity;	
				}

			private:
				std::shared_ptr<ComponentManager> mComponentManager;
				std::shared_ptr<EntityManager> mEntityManager;
				std::shared_ptr<EventManager> mEventManager;
				std::shared_ptr<SystemManager> mSystemManager;
				std::shared_ptr<TextureManager> mTextureManager;
				std::shared_ptr<StateManager> mStateManager;
				
				 
			};

		}
	}