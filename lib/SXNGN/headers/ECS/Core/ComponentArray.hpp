#pragma once

#include "Types.hpp"
#include <array>
#include <cassert>
#include <unordered_map>
#include <ECS/Core/Component.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
/**
class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
	virtual void EntityDestroyed(Entity entity) = 0;
};
**/

using ECS_Component = SXNGN::ECS::A::ECS_Component;
using ComponentTypeEnum = SXNGN::ECS::A::ComponentTypeEnum;

class ComponentArray// : public IComponentArray
{
public:
	void InsertData(Entity entity, ECS_Component *component)
	{
		std::lock_guard<std::mutex> guard(master_component_array_guard);//Wait until array is safe to operate on 
		array_wide_operation_in_progress.store(true);

		if (mComponentArray[entity])
		{
			SDL_LogError(1, "InsertData : Component added to same entity more than once: Type %d", component->get_component_type());
		}
		else
		{
			mComponentArray[entity] = component;
			++mSize;
		}
		array_wide_operation_in_progress.store(false);
	}

	void RemoveData(Entity entity, bool have_lock = false)
	{
		ECS_Component* to_remove = ExtractComponent(entity, have_lock);
		--mSize;
		delete to_remove;
	}

	void EntityDestroyed(Entity entity, bool have_lock = false)
	{
		RemoveData(entity, have_lock = false);
	}

	/// <summary>
	/// Remove the data from the array, return the pointer to it
	/// </summary>
	/// <param name="entity"></param>
	/// <returns></returns>
	ECS_Component* ExtractData(Entity entity, bool have_lock = false)
	{
		ECS_Component* to_extract = ExtractComponent(entity, have_lock);
		return to_extract;
	}

	//Thread safe - get a read-only copy of the data
	const ECS_Component* GetDataReadOnly(Entity entity)
	{
		const ECS_Component* to_return = nullptr;
		
		std::lock_guard<std::mutex> guard(master_component_array_guard);
		mComponentMutexes[entity].lock();//Wait until data is available (no other theadss have checked it out)
		if (mComponentArray[entity])
		{
			auto component_ref = mComponentArray[entity];
			to_return = (const ECS_Component*)(component_ref); //make a read-only copy of the data to return
		}
		mComponentMutexes[entity].unlock();
		return to_return;
	} 

	const std::array<ECS_Component*, MAX_ENTITIES> GetAllDataReadOnly()
	{
		
		std::lock_guard<std::mutex> guard(master_component_array_guard);
		return  mComponentArray;

	}

	std::array<ECS_Component*, MAX_ENTITIES>& CheckOutAllData()
	{
		master_component_array_guard.lock();
		return  mComponentArray;
	}

	void CheckInAllData()
	{
		master_component_array_guard.unlock();
	}

	//deprecated
	const ECS_Component* TryGetDataReadOnly(Entity entity)
	{
		return GetDataReadOnly(entity);
	}

	//Get a modifiable instance of the component for this entity
	//Do not allow any changes to the component array until CheckInData() is called
	//Mutex corresponding to this data is locked until checked back in
	//DO NOT DELETE any pointer returned from this
	ECS_Component* CheckoutData(Entity entity)
	{
		//Lock the array until the data is checked back in (under "authority" of master_component_array_guard)
		//(Thread entering this function will block here until it can get the lock)
		
		std::lock_guard<std::mutex> guard(master_component_array_guard);

		ECS_Component* to_return = nullptr;

		if (entity < 0 || entity > MAX_ENTITIES)
		{
			return to_return;
		}

		if (mComponentInUse[entity] == true)
		{
			SDL_LogCritical(1, "CheckoutData: Component of entity %d already in use!", entity);
			abort();
		}
		
		mComponentMutexes[entity].lock();//Wait until data is available (no other theadss have checked it out)
		if (mComponentArray[entity])
		{
			mComponentInUse[entity] = true;
			component_specific_operations_in_progress++;
			to_return = mComponentArray[entity];
		}
		else
		{
			mComponentMutexes[entity].unlock();
		}		
		 //DO NOT UNLOCK - CheckInData will do this - should remain locked until then
		return to_return;
	} 

	//See above
	void CheckInData(Entity entity)
	{
		//Lock the array until the data is checked back in (under "authority" of master_component_array_guard)
		//(Thread entering this function will block here until it can get the lock)
		std::lock_guard<std::mutex> guard(master_component_array_guard);
		//if (mEntityToIndexMap.find(entity) == mEntityToIndexMap.end())
		//{
		//	SDL_LogCritical(1, "CheckInData: Entity %d not checked out or does not exist\n", entity);
		//	abort();
		//}
		if (component_specific_operations_in_progress.load() <= 0)
		{
			SDL_LogCritical(1, "CheckInData: component_specific_operations_in_progress <= 0");
			abort();
		}
		component_specific_operations_in_progress--;
		mComponentInUse[entity] = false;
		mComponentMutexes[entity].unlock();
	}



private:

	ECS_Component* ExtractComponent(Entity entity, bool have_lock = false)
	{
		//lock the master lock. Checking in/out components will wait until this is free.
		if (!have_lock)
		{
			std::lock_guard<std::mutex> guard(master_component_array_guard);
		}
		
		//if (mEntityToIndexMap.find(entity) == mEntityToIndexMap.end())
		//{
		//	return nullptr;
		//}
		while (component_specific_operations_in_progress.load() > 0 && array_wide_operation_in_progress.load() == true)
		{
			//waiting
			//fixme use conditional here
		}
		array_wide_operation_in_progress.store(true);

		//lock the lock corresponding to entity to extract
		try
		{
			mComponentMutexes[entity].lock();
		}
		catch (std::system_error e)
		{
			SDL_LogCritical(1, "ExtractComponent: No component operations in progress, but component to be deleted has locked mutex. A function did not check in data after checking it out.");
			abort();
		}
		
		
		//size_t indexOfRemovedEntity = mEntityToIndexMap[entity];
		//size_t indexOfLastElement = mSize - 1;
		ECS_Component* component_to_extract = nullptr;
		if (mComponentArray[entity])
		{
			component_to_extract = mComponentArray[entity];
			mComponentArray[entity] = nullptr;
		}
		// Copy element at end into deleted element's place to maintain density
		//mComponentArray[indexOfRemovedEntity] = mComponentArray[indexOfLastElement];

		// Update map to point to moved spot
		// !Entity entityOfLastElement = mIndexToEntityMap[indexOfLastElement];
		// !mEntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
		// !mIndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

		// !mEntityToIndexMap.erase(entity);
		// !mIndexToEntityMap.erase(indexOfLastElement);

		--mSize;
		mComponentMutexes[entity].unlock();
		array_wide_operation_in_progress.store(false);
		return component_to_extract;
	}

	std::array<ECS_Component*, MAX_ENTITIES> mComponentArray{};
	//std::unordered_map<Entity, size_t> mEntityToIndexMap{};
	//std::unordered_map<size_t, Entity> mIndexToEntityMap{};
	size_t mSize{};
	std::mutex master_component_array_guard;//mutex used to guard this CompnentArray's data
	std::array<std::mutex, MAX_ENTITIES> mComponentMutexes{};//locks for each individual component
	std::array<bool, MAX_ENTITIES> mComponentInUse{};//locks for each individual component
	//std::array<std::unique_lock<std::mutex>, MAX_ENTITIES> mComponentUniqueLocks{};//locks for each individual component
	std::atomic<bool> array_wide_operation_in_progress = false; //flag that an array-wide operation is ongoing, ie, something that will affect mEntityToIndexMap. Not safe for any thread to access data in the array in any way until this is complete.
	std::atomic<int> component_specific_operations_in_progress = 0; //flag that any thread has data checked out

};
