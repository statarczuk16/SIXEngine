#pragma once

#include <ECS/Components/Renderable.hpp>
#include "ECS/Core/System.hpp"
#include <memory>
#include <SDL.h>
#include <ECS/Components/Camera.hpp>


class Event;
using ECS_Camera = SXNGN::ECS::Components::Camera;
using Renderable = SXNGN::ECS::Components::Renderable;


class Renderer_System : public System
{
public:
	void Init();

	void Update(float dt);

private:

	bool object_in_view(ECS_Camera camera, SDL_Rect object);

	SDL_Rect determine_camera_lens_scaled(ECS_Camera camera);

	SDL_Rect determine_camera_lens_unscaled(ECS_Camera camera);

	void Render(const Renderable* renderable, ECS_Camera camera);

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
