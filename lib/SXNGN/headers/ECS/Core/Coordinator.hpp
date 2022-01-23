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



namespace SXNGN {
	namespace ECS {
		namespace A {
			using ECS_Component = A::ECS_Component;


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
				Entity CreateEntity(bool quiet = false)
				{
					return mEntityManager->CreateEntity(quiet);
				}

				void DestroyEntity(Entity entity)
				{
					mEntityManager->DestroyEntity(entity);

					mSystemManager->EntityDestroyed(entity);

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
				void GameStateChanged(std::forward_list<ComponentTypeEnum> active_game_states, bool quiet = true)
				{
					printf("Changing game state\n");
					mStateManager->gameStateChanged(active_game_states);
					Signature game_state_signature;
					for (auto state : active_game_states)
					{
						game_state_signature.set(GetComponentType(state));
					}

					auto entity_signatures = mEntityManager->GetAllEntitySignatures();

					mSystemManager->GameStateChanged(game_state_signature, entity_signatures, quiet);
				}

				std::forward_list<ComponentTypeEnum> GetActiveGameStates()
				{
					return mStateManager->getActiveGameStates();
				}

				/**
				ECS_Component* GetComponent(Entity entity, ComponentTypeEnum component_type)
				{
					return mComponentManager->GetComponent(entity, component_type);
				}

				ECS_Component* TryGetComponent(Entity entity, ComponentTypeEnum component_type)
				{
					return mComponentManager->TryGetComponent(entity, component_type);
				}
				**/

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
				std::shared_ptr<A::ExternEntity> Extract_Entity_From_ECS(Entity entity)
				{

					auto extracted_components = Extract_Entity_Components_From_ECS(entity);
					auto extracted_entity = std::make_shared<A::ExternEntity>(entity, extracted_components);
					return extracted_entity;
				}

				/// <summary>
				/// Cache an extern_entity into a space - outside of the ECS
				/// </summary>
				/// <param name="entity_to_store"></param>
				/// <param name="state_to_store_in"></param>
				void Space_Extern_Entity(std::shared_ptr<A::ExternEntity> entity_to_store, std::string state_to_store_in = "Temp")
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
					std::vector< std::shared_ptr<A::ExternEntity>> entity_array = mStateManager->retrieveSpaceEntities(state_to_dump_from, destroy_after);
					for (auto entity : entity_array)
					{
						Dump_Spaced_Entity_To_ECS(entity);
					}
				}

				/// <summary>
				/// Add an ExternEntity back into the system
				/// </summary>
				/// <param name="entity_to_dump"></param>
				void Dump_Spaced_Entity_To_ECS(std::shared_ptr<A::ExternEntity> entity_to_dump)
				{
					Entity new_id = mEntityManager->CreateEntity();
					for (auto component : entity_to_dump->entity_components_)
					{
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
				json Entity_To_JSON(Entity entity)
				{
					std::vector<json> json_components;
					std::vector<const ECS_Component*> components = Get_All_Entity_Data_Read_Only(entity);
					for (auto component_ptr : components)
					{
						json js = JSON_Utils::component_to_json(component_ptr);
						json_components.push_back(js);
					}
					A::ExternJSONEntity extern_entity(entity, json_components);
					json ret = extern_entity;
					return ret;
				}

				json JSON_To_Entity(json j)
				{
					std::vector<json> json_components;
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
					if (component_type_enum != ComponentTypeEnum::JSON_ENTITY)
					{
						SDL_LogError(1, "Cannot Read Incoming JSON because is not JSON_ENTITY: ");
						std::cout << j.dump(4) << std::endl;
						return nullptr;
					}
					auto list_of_components = j.find("entity_components_");
					if (list_of_components == j.end())
					{
						return nullptr;
					}
					for (auto component_pair : *list_of_components)
					{
						std::cout << component_pair.dump(4) << std::endl;//fixme almost done
					}

					
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
}