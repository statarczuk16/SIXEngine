#pragma once

#include "ECS/Core/System.hpp"
#include <memory>




class Event;



class Renderer_System : public System
{
public:
	void Init();

	void Update(float dt);

private:


};

//takes in pre_renderables sprite_factory and creates a renderable/sprite
class Sprite_Factory_System : public System
{
public:
	void Init();

	void Update(float dt);

private:


};

//Takes in pre_sprite_factory components and creates a sprite_factory_system component
class Sprite_Factory_Creator_System : public System
{
public:
	void Init();

	void Update(float dt);

private:


};
