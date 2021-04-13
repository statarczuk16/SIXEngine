#pragma once

#include "ECS/Core/Types.hpp"
#include <array>
#include <cassert>
#include <queue>


class EntityManager
{
public:
	EntityManager()
	{
		for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
		{
			mAvailableEntities.push(entity);
		}
	}

	Entity CreateEntity(bool quiet = false)
	{
		assert(mLivingEntityCount < MAX_ENTITIES && "Too many entities in existence.");

		if (mAvailableEntities.empty())
		{
			SDL_LogCritical(1, "Out of available entities");
			abort();
		}
		Entity id = mAvailableEntities.front();
		mAvailableEntities.pop();
		livingEntities[id] = true;
		++mLivingEntityCount;

		return id;
	}

	void DestroyEntity(Entity entity)
	{
		assert(entity < MAX_ENTITIES && "Entity out of range.");

		mSignatures[entity].reset();
		if (livingEntities[entity])
		{
			livingEntities[entity] = false;
			--mLivingEntityCount;
			mAvailableEntities.push(entity);
		}

	}

	bool EntityLiving(Entity entity)
	{
		if (entity > MAX_ENTITIES || entity < 0)
		{
			return false;
		}
		return livingEntities[entity];
	}

	void SetSignature(Entity entity, Signature signature)
	{
		assert(entity < MAX_ENTITIES && "Entity out of range.");

		mSignatures[entity] = signature;
	}

	Signature GetSignature(Entity entity)
	{
		assert(entity < MAX_ENTITIES && "Entity out of range.");

		return mSignatures[entity];
	}

	std::vector<std::pair<Entity, Signature>> GetAllEntitySignatures()
	{
		std::vector<std::pair<Entity, Signature>> ret;
		for (int i = 0; i < mSignatures.size(); i++)
		{
			if (livingEntities[i] == true)
			{
				ret.push_back(std::make_pair(i, mSignatures[i]));
			}
			if (ret.size() >= mLivingEntityCount)
			{
				return ret;
			}	
		}
		return ret;
	}

private:
	std::queue<Entity> mAvailableEntities{};
	std::array<Signature, MAX_ENTITIES> mSignatures{};
	uint32_t mLivingEntityCount{};
	std::array<bool, MAX_ENTITIES> livingEntities{false};
};
