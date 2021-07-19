#pragma once

#include <ECS/Components/Renderable.hpp>
#include "ECS/Core/System.hpp"
#include <memory>
#include <SDL.h>
#include <ECS/Components/CameraComponent.hpp>
#include <ECS/Components/UI.hpp>


class Event;
using ECS_Camera = SXNGN::ECS::A::CameraComponent;
using Renderable = SXNGN::ECS::A::Renderable;
namespace SXNGN::ECS::A {

	class Renderer_System : public System
	{
	public:
		void Init();

		void Update(float dt);

		void Draw_GUI();

		void Draw_GUI_Components(std::map<UILayer, std::vector<std::shared_ptr<UIContainerComponent>>> layer_to_components);

		void Draw_GUI_Component(SDL_Renderer* gRenderer, std::shared_ptr<UIContainerComponent> component_in_layer);

	private:

		

		void Render(const Renderable* renderable, std::shared_ptr<ECS_Camera> camera);

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
}