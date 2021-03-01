#pragma once

#include "ECS/Core/System.hpp"


class PhysicsSystem : public System
{
public:
	void Init();

	void Update(float dt);
};
