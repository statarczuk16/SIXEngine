#pragma once

#include "Types.hpp"
#include <array>
#include <cassert>
#include <unordered_map>
#include <ECS/Core/Component.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>
/**
class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
	virtual void EntityDestroyed(Entity entity) = 0;
};
**/

using ECS_Component = SXNGN::ECS::Components::ECS_Component;
using ComponentTypeEnum = SXNGN::ECS::Components::ComponentTypeEnum;

class ComponentArray// : public IComponentArray
{
public:
	void InsertData(Entity entity, ECS_Component *component)
	{
		assert(mEntityToIndexMap.find(entity) == mEntityToIndexMap.end() && "Component added to same entity more than once.");

		// Put new entry at end
		size_t newIndex = mSize;
		mEntityToIndexMap[entity] = newIndex;
		mIndexToEntityMap[newIndex] = entity;
		mComponentArray[newIndex] = component;
		++mSize;
	}

	void RemoveData(Entity entity)
	{
		assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Removing non-existent component.");

		// Copy element at end into deleted element's place to maintain density
		size_t indexOfRemovedEntity = mEntityToIndexMap[entity];
		size_t indexOfLastElement = mSize - 1;
		if (mComponentArray[indexOfRemovedEntity])
		{
			delete mComponentArray[indexOfRemovedEntity];
			mComponentArray[indexOfRemovedEntity] = nullptr;
		}
		mComponentArray[indexOfRemovedEntity] = mComponentArray[indexOfLastElement];

		// Update map to point to moved spot
		Entity entityOfLastElement = mIndexToEntityMap[indexOfLastElement];
		mEntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
		mIndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

		mEntityToIndexMap.erase(entity);
		mIndexToEntityMap.erase(indexOfLastElement);

		--mSize;
	}

	//If you are for some reason extremely confident the data exists
	ECS_Component* GetData(Entity entity)
	{
		assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Retrieving non-existent component.");

		return mComponentArray[mEntityToIndexMap[entity]];
	}


	

	ECS_Component* TryGetData(Entity entity)
	{
		if (mEntityToIndexMap.find(entity) == mEntityToIndexMap.end())
		{
			return nullptr;
		}

		return mComponentArray[mEntityToIndexMap[entity]];
	}


	//Thread safe - get a read-only copy of the data

	const ECS_Component* GetDataReadOnly(Entity entity)
	{
		std::lock_guard<std::mutex> guard(component_array_guard);//Wait until data is available (no other theadss have checked it out)
		assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Retrieving non-existent component.");
		auto component_ref = mComponentArray[mEntityToIndexMap[entity]];
		const ECS_Component* const_ref(component_ref); //make a read-only copy of the data to return
		return const_ref;
	} 

	//Get a modifiable instance of the component for this entity
	//Do not allow any changes to the component array until CheckInData() is called
	//When checking out data, you take the with you by which the data can be accessed (unique_lock).
	//DO NOT DELETE any pointer returned from this
	std::pair<ECS_Component*, std::unique_ptr<std::unique_lock<std::mutex>>> CheckoutData(Entity entity)
	{
		//Lock the array until the data is checked back in (under "authority" of component_array_guard)
		//(Thread entering this function will block here until it can get the lock)
		std::unique_ptr<std::unique_lock<std::mutex>> check_out_component_lock = std::make_unique< std::unique_lock<std::mutex>>(component_array_guard);
		if (mEntityToIndexMap.find(entity) == mEntityToIndexMap.end())
		{
			//dont lock if there is nothing to return
			check_out_component_lock->unlock();
			return std::make_pair(nullptr, nullptr);
		}
		//make a pointer to the lock so it can be returned to user
		auto raw_ptr = mComponentArray[mEntityToIndexMap[entity]];
		return std::make_pair(raw_ptr, std::move(check_out_component_lock));
	} 

	//See above
	//Return the key so others can check out data
	void CheckInData(Entity entity, std::unique_ptr<std::unique_lock<std::mutex>> key)
	{
		//(If they return a broken key, unlock the vault so others can get in)
		if (key == nullptr)
		{
			printf("CheckInData:: nullptr key return. Unlocking mutex");
			component_array_guard.unlock();
		}
		//use returned key to unlock the data
		if (key->owns_lock())
		{
			key->unlock();
		}
		key = nullptr;
	}

	void EntityDestroyed(Entity entity)
	{
		if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end())
		{
			RemoveData(entity);
		}
	}

private:
	std::array<ECS_Component*, MAX_ENTITIES> mComponentArray{};
	std::unordered_map<Entity, size_t> mEntityToIndexMap{};
	std::unordered_map<size_t, Entity> mIndexToEntityMap{};
	size_t mSize{};
	std::mutex component_array_guard;//mutex used to guard this CompnentArray's data

};
