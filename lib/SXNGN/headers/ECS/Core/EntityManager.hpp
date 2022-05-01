#pragma once

#include "ECS/Core/Types.hpp"
#include <array>
#include <cassert>
#include <queue>
#include <Constants.h>
#include <sole.hpp>
#include <Constants.h>




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

	Entity CreateEntity(sole::uuid uuid_in = SXNGN::BAD_UUID, bool quiet = false)
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
		if (uuid_in == SXNGN::BAD_UUID)
		{
			uuid_in = sole::uuid1();
		}
		mUUIDToEntityMap[uuid_in] = id;
		mEntityToUUIDMap[id] = uuid_in;
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
			sole::uuid uuid_to_delete = mEntityToUUIDMap[entity];
			mUUIDToEntityMap.erase(uuid_to_delete);
			mEntityToUUIDMap.erase(entity);
			mAvailableEntities.push(entity);
		}

	}

	Entity GetEntityFromUUID(sole::uuid uuid)
	{
		if (mUUIDToEntityMap.count(uuid) > 0)
		{
			Entity entity = mUUIDToEntityMap[uuid];
			return entity;
		}
		else
		{
			return -1;
		}
	}

	sole::uuid GetUUIDFromEntity(Entity entity)
	{
		if (mEntityToUUIDMap.count(entity) > 0)
		{
			return mEntityToUUIDMap[entity];
		}
		else
		{
			return SXNGN::BAD_UUID;
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

	uint32_t GetLivingEntityCount()
	{
		return mLivingEntityCount;
	}

private:
	std::queue<Entity> mAvailableEntities{};
	std::array<Signature, MAX_ENTITIES> mSignatures{};
	uint32_t mLivingEntityCount{};
	std::array<bool, MAX_ENTITIES> livingEntities{false};
	std::unordered_map<sole::uuid, Entity> mUUIDToEntityMap;
	std::unordered_map<Entity, sole::uuid> mEntityToUUIDMap;
};
