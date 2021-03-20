#pragma once

#include "ECS/Core/System.hpp"
#include "ECS/Core/Types.hpp"
#include <cassert>
#include <memory>
#include <unordered_map>
#include <SDL.h>

namespace SXNGN::ECS::A {
	class SystemManager
	{
	public:
		template<typename T>
		std::shared_ptr<T> RegisterSystem()
		{
			const char* typeName = typeid(T).name();

			assert(mSystems.find(typeName) == mSystems.end() && "Registering system more than once.");

			auto system = std::make_shared<T>();
			mSystems.insert({ typeName, system });
			return system;
		}

		template<typename T>
		void SetSignatureActable(Signature signature)
		{
			const char* typeName = typeid(T).name();

			assert(mSystems.find(typeName) != mSystems.end() && "A used before registered.");

			mSignaturesActableAND.insert({ typeName, signature });
			mGameStateSignatureOR.insert({ typeName, signature });
		}

		template<typename T>
		void SetSignatureOfInterest(Signature signature)
		{
			const char* typeName = typeid(T).name();

			assert(mSystems.find(typeName) != mSystems.end() && "A used before registered.");

			mSignaturesOfInterestOR.insert({ typeName, signature });
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

		/// <summary>
		/// Currently no difference between EntityDestroyed
		/// </summary>
		/// <param name="entity"></param>
		void EntityDeactivated(Entity entity)
		{
			for (auto const& pair : mSystems)
			{
				auto const& system = pair.second;


				system->m_actable_entities.erase(entity);
				system->m_entities_of_interest.erase(entity);
			}
		}

		/// <summary>
		/// Currently no difference between EntityDestroyed
		/// </summary>
		/// <param name="entity"></param>
		void GameStateChanged(Signature game_state_signature, std::vector<std::pair<Entity, Signature>> living_entity_signatures, bool quiet = true)
		{
			auto start = std::chrono::high_resolution_clock::now();
			if (true)//!quiet)
			{
				SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "GameStateChanged: new signature: %d", game_state_signature);
			}
			for (auto const& pair : mSystems)
			{
				auto const& system = pair.second;
				auto const& type = pair.first;
				//update the game state signature, so now only entities with any of the matching game state components should match an OR compare
				//IE if Signature contains GameStateMain + GameMenuMain, only components having either one of those components should be visible to the system
				mGameStateSignatureOR[type] = game_state_signature;
			}
			//now that the system signature is updated, have to go through all of the entities and compare
			for (auto entity_sig_pair : living_entity_signatures)
			{
				EntitySignatureChanged(entity_sig_pair.first, entity_sig_pair.second, quiet);
			}
			auto stop = std::chrono::high_resolution_clock::now();
			if (true)//(!quiet)
			{
				auto duration = std::chrono::duration_cast<std::chrono::milliseconds > (stop - start);
				SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "GameStateChanged: Updated %d Entities across %d Systems in %d ms", living_entity_signatures.size(), mSystems.size(), duration);
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
				auto const& systemSignatureActAND = mSignaturesActableAND[type];
				auto const& systemGameStateSignatureActOR = mGameStateSignatureOR[type];
				auto const& systemSignatureInterestOR = mSignaturesOfInterestOR[type];
				Signature game_state_result_or;
				game_state_result_or = entitySignature & systemGameStateSignatureActOR;

				//First pass, if the entity is not in an active game state, it can't be seen by any system
				if (!(game_state_result_or.any())) //Logical OR to see if entity has ANY of the components of matching the GameStateOR Signature
				{
					system->m_entities_of_interest.erase(entity);
					system->m_actable_entities.erase(entity);
					continue;
				}

				//Logical AND to see if entity has ALL of the components of matching the AND Signature
				if ((entitySignature & systemSignatureActAND) == systemSignatureActAND)
				{
					system->m_actable_entities.insert(entity);
					if (!quiet)
					{
						SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "EntitySignature Changed: Entity ID %d: Added to A ActablesAND: Component %s", entity, type);
					}
				}
				else
				{
					system->m_actable_entities.erase(entity);
				}
				//Logical OR to see if entity has ANY of the components of interest to this system
				// (A system may need data from other entities even if it does not operate on them in its main loop)
				Signature or_result;
				or_result = entitySignature & systemSignatureInterestOR;
				if (or_result.any())
				{
					system->m_entities_of_interest.insert(entity);
					if (!quiet)
					{
						SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "EntitySignature Changed: Entity ID %d: Added to A InterestsOR: A: %s", entity, type);

					}
				}
				else
				{
					system->m_entities_of_interest.erase(entity);
				}

			}
		}
		

	private:
		//Entities must have ALL components in signature to be added to entities_actable list of system
		std::unordered_map<const char*, Signature> mSignaturesActableAND{};
		//Entities must have ANY component in game state signature to be added to entities_actable list of system (is the entity part of any of the active game states)
		std::unordered_map<const char*, Signature> mGameStateSignatureOR{};
		//Entities must have ANY component in signature to be added to entities_of_interest of system
		std::unordered_map<const char*, Signature> mSignaturesOfInterestOR{};
		std::unordered_map<const char*, std::shared_ptr<System>> mSystems{};
	};
}