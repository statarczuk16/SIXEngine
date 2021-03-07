#pragma once

#include <ECS/Core/ComponentArray.hpp>
#include <ECS/Core/Types.hpp>
#include <memory>
#include <unordered_map>
#include <vector>


class ComponentManager
{

public:

	void RegisterComponent(ComponentTypeEnum component_type)
	{

		assert(mComponentTypes.find(component_type) == mComponentTypes.end() && "Registering component type more than once.");

		mComponentTypes.insert({ component_type, mNextComponentType});
		mComponentArrays.insert({ component_type, std::make_shared<ComponentArray>() });

		++mNextComponentType;
	}

	ComponentTypeHash GetComponentType(ComponentTypeEnum component_type)
	{

		if (mComponentTypes.find(component_type) == mComponentTypes.end())
		{
			const char* component_type_str = typeid(component_type).name();
			printf("Component not registered before use: %s\n", component_type_str);
		}

		assert(mComponentTypes.find(component_type) != mComponentTypes.end());

		return mComponentTypes[component_type];
	}


	void AddComponent(Entity entity, ECS_Component *component)
	{
		ComponentTypeEnum component_type = component->get_component_type();
		GetComponentArray(component_type)->InsertData(entity, component);
	}

	void RemoveComponent(Entity entity, ComponentTypeEnum component_type)
	{
		GetComponentArray(component_type)->RemoveData(entity);
	}


	ECS_Component* GetComponent(Entity entity, ComponentTypeEnum component_type)
	{
		return GetComponentArray(component_type)->GetData(entity);
	}

	ECS_Component* TryGetComponent(Entity entity, ComponentTypeEnum component_type)
	{
		auto ptr = GetComponentArray(component_type)->TryGetData(entity);
		if (ptr->get_component_type() != component_type)
		{
			printf("ComponentManager::TryGetComponent wrong component type in array - expected %2d, received %2d",component_type, ptr->get_component_type());
			return nullptr;
		}
		return ptr;
	}


	const ECS_Component* GetComponentReadOnly(Entity entity, ComponentTypeEnum component_type)
	{
		return GetComponentArray(component_type)->GetDataReadOnly(entity);
	}


	std::pair<ECS_Component*, std::unique_ptr<std::unique_lock<std::mutex>>> CheckOutComponent(Entity entity, ComponentTypeEnum component_type)
	{
		return GetComponentArray(component_type)->CheckoutData(entity);
	}

	void CheckInComponent(ComponentTypeEnum component_type, Entity entity, std::unique_ptr<std::unique_lock<std::mutex>> key)
	{
		return GetComponentArray(component_type)->CheckInData(entity, std::move(key));
	}




	//fixme add some mutex method here to manage requests for components
	//Use template to return value of requested component

	void EntityDestroyed(Entity entity)
	{
		for (auto const& pair : mComponentArrays)
		{
			auto const& component = pair.second;

			component->EntityDestroyed(entity);
		}
	}

	std::vector<SXNGN::ECS::Components::ECS_Component*> get_all_entity_data(Entity entity)
	{
		std::vector<SXNGN::ECS::Components::ECS_Component*> return_components;
		for (auto component_type : mComponentTypes)
		{
			ComponentTypeEnum component_type_enum = component_type.first;
			auto comp_arr = (mComponentArrays[component_type_enum]);
			auto component = comp_arr->TryGetData(entity);
			if (component)
			{
				return_components.push_back(component);
			}
		}
		return return_components;
	}


private:

	std::unordered_map<ComponentTypeEnum, ComponentTypeHash> mComponentTypes{};
	std::unordered_map<ComponentTypeEnum, std::shared_ptr<ComponentArray>> mComponentArrays{};
	ComponentTypeHash mNextComponentType{};

	std::shared_ptr<ComponentArray> GetComponentArray(ComponentTypeEnum component_type)
	{

		assert(mComponentTypes.find(component_type) != mComponentTypes.end() && "Component not registered before use.");

		return (mComponentArrays[component_type]);
	}
};