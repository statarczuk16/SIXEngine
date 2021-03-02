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
	void Init()
	{
		mComponentManager = std::make_shared<ComponentManager>();
		mEntityManager = std::make_shared<EntityManager>();
		mEventManager = std::make_shared<EventManager>();
		mSystemManager = std::make_shared<SystemManager>();
		mTextureManager = std::shared_ptr<TextureManager>();
	}


	// Entity methods
	Entity CreateEntity()
	{
		return mEntityManager->CreateEntity();
	}

	void DestroyEntity(Entity entity)
	{
		mEntityManager->DestroyEntity(entity);

		mComponentManager->EntityDestroyed(entity);

		mSystemManager->EntityDestroyed(entity);
	}

	std::vector<SXNGN::ECS::Components::ECS_Component> get_all_entity_data(Entity entity)
	{
		
	}

	// Component methods
	
	void RegisterComponent(ComponentTypeEnum component_type)
	{
		mComponentManager->RegisterComponent(component_type);
	}

	void AddComponent(Entity entity, ECS_Component *component)
	{
		mComponentManager->AddComponent(entity, component);

		auto signature = mEntityManager->GetSignature(entity);
		signature.set(mComponentManager->GetComponentType(component->get_component_type()), true);
		mEntityManager->SetSignature(entity, signature);

		mSystemManager->EntitySignatureChanged(entity, signature);
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
