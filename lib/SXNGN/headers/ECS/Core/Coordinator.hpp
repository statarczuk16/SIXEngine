#pragma once

#include "ECS/Core/ComponentManager.hpp"
#include "ECS/Core/EntityManager.hpp"
#include "ECS/Core/EventManager.hpp"
#include "ECS/Core/SystemManager.hpp"
#include "ECS/Core/Types.hpp"
#include <memory>
#include <ECS/Core/Component.hpp>
#include <ECS/Core/TextureManager.hpp>


using ECS_Component = SXNGN::ECS::Components::ECS_Component;


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


	// Entity methods
	Entity CreateEntity(bool quiet = false)
	{
		return mEntityManager->CreateEntity(false);
	}

	void DestroyEntity(Entity entity)
	{
		mEntityManager->DestroyEntity(entity);

		mComponentManager->EntityDestroyed(entity);

		mSystemManager->EntityDestroyed(entity);
	}

	std::vector<const ECS_Component*> Get_All_Entity_Data_Read_Only(Entity entity)
	{
		return mComponentManager->Get_All_Entity_Data_Read_Only(entity);
	}

	// Component methods
	
	void RegisterComponent(ComponentTypeEnum component_type)
	{
		mComponentManager->RegisterComponent(component_type);
	}

	void AddComponent(Entity entity, ECS_Component *component, bool quiet = false)
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

	ECS_Component* GetComponent(Entity entity, ComponentTypeEnum component_type)
	{
		return mComponentManager->GetComponent(entity, component_type);
	}

	ECS_Component* TryGetComponent(Entity entity, ComponentTypeEnum component_type)
	{
		return mComponentManager->TryGetComponent(entity, component_type);
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
	/// Thread safe access to data. Thread asks for component of an entity and is given the data and the key that is used to access it.
	/// (Other threads cannot access that same data without the key)
	/// The only function of they is that it must be returned with CheckInComponent before other threads can access the data.
	/// Do not ever use without calling CheckInComponent when done.
	/// </summary>
	/// <param name="entity"></param>
	/// <param name="component_type"></param>
	/// <returns></returns>
	std::pair<ECS_Component*, std::unique_ptr<std::unique_lock<std::mutex>>> CheckOutComponent(Entity entity, ComponentTypeEnum component_type)
	{
		return mComponentManager->CheckOutComponent(entity, component_type);
	}

	/// <summary>
	/// Used by a thread to return checked-out data.
	/// </summary>
	/// <param name="entity"></param>
	/// <param name="component_type"></param>
	/// <param name="data_and_key"></param>
	void CheckInComponent(ComponentTypeEnum component_type, Entity entity, std::unique_ptr<std::unique_lock<std::mutex>> key)
	{
		return mComponentManager->CheckInComponent(component_type, entity, std::move(key));
	}
	ComponentTypeHash GetComponentType(ComponentTypeEnum component_type)
	{
		return mComponentManager->GetComponentType(component_type);
	}

	template<typename T>
	// System methods
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

private:
	std::shared_ptr<ComponentManager> mComponentManager;
	std::shared_ptr<EntityManager> mEntityManager;
	std::shared_ptr<EventManager> mEventManager;
	std::shared_ptr<SystemManager> mSystemManager;
	std::shared_ptr<TextureManager> mTextureManager;
};
