#pragma once

#include "ECS/Core/Types.hpp"
#include <set>


class System
{
public:
	//List of entities that contain all components needed by this system. Maintained by SystemManager
	std::set<Entity> mEntities;
};
