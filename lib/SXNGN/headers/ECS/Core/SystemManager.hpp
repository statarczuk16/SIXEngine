#pragma once

#include "ECS/Core/System.hpp"
#include "ECS/Core/Types.hpp"
#include <cassert>
#include <memory>
#include <unordered_map>
#include <SDL.h>


class SystemManager
{
public:
	template<typename T>
	std::shared_ptr<T> RegisterSystem()
	{
		const char* typeName = typeid(T).name();

		assert(mSystems.find(typeName) == mSystems.end() && "Registering system more than once.");

		auto system = std::make_shared<T>();
		mSystems.insert({typeName, system});
		return system;
	}

	template<typename T>
	void SetSignatureActable(Signature signature)
	{
		const char* typeName = typeid(T).name();

		assert(mSystems.find(typeName) != mSystems.end() && "System used before registered.");

		mSignaturesActable.insert({typeName, signature});
	}

	template<typename T>
	void SetSignatureOfInterest(Signature signature)
	{
		const char* typeName = typeid(T).name();

		assert(mSystems.find(typeName) != mSystems.end() && "System used before registered.");

		mSignaturesOfInterest.insert({ typeName, signature });
	}

	void EntityDestroyed(Entity entity)
	{
		for (auto const& pair : mSystems)
		{
			auto const& system = pair.second;


			system->m_actable_entities.erase(entity);
			system->m_entities_of_interest.erase(entity);
		}
	}

	void EntitySignatureChanged(Entity entity, Signature entitySignature, bool quiet = false)
	{
		if (!quiet)
		{
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "EntitySignature Changed: Entity ID: %2d : New Signature: %d", entity, entitySignature);

		}

		for (auto const& pair : mSystems)
		{
			auto const& type = pair.first;
			auto const& system = pair.second;
			auto const& systemSignatureAct = mSignaturesActable[type];
			auto const& systemSignatureInterest = mSignaturesOfInterest[type];
			//Logical AND to see if entity has all the components of this system
			if ((entitySignature & systemSignatureAct) == systemSignatureAct)
			{
				if (!quiet)
				{
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "EntitySignature Changed: Entity ID %d: Added to System Actables: System: %s", entity, type);

				}
				system->m_actable_entities.insert(entity);
			}
			else
			{
				system->m_actable_entities.erase(entity);
				//Logical OR to see if entity has ANY of the components of interest to this system
				// (System may need data from other entities even if it does not operate on them in its main loop)
				Signature or_result;
				or_result = entitySignature & systemSignatureInterest;
				if (or_result.any())
				{
					system->m_entities_of_interest.insert(entity);
					if (!quiet)
					{
						SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "EntitySignature Changed: Entity ID %d: Added to System Interests: System: %s", entity, type);

					}
				}
				else
				{
					system->m_entities_of_interest.erase(entity);
				}
			}



			

			
		}
	}

private:
	//Entities must have ALL components in signature to be added to entities_actable list of system
	std::unordered_map<const char*, Signature> mSignaturesActable{};
	//Entities must have ANY component in signature to be added to entities_of_interest of system
	std::unordered_map<const char*, Signature> mSignaturesOfInterest{};
	std::unordered_map<const char*, std::shared_ptr<System>> mSystems{};
};
