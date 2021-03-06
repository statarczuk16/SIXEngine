
#include <ECS/Components/CameraComponent.hpp>
#include <ECS/Core/Coordinator.hpp>
#include <Database.h>
#include <ECS/Components/Renderable.hpp>
#include <ECS/Systems/CameraSystem.hpp>

namespace SXNGN::ECS::A {

	void Camera_System::Init()
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Camera_System Init");
	}

	/// <summary>
	/// Camera's goals: Don't do much, just follow around the target.
	/// </summary>
	/// <param name="dt"></param>
	void Camera_System::Update(float dt)
	{

		auto camera = A::CameraComponent::get_instance();
		if (camera == nullptr)
		{
			return;
		}
		auto gCoordinator = *Database::get_coordinator();

		//search for events
		for (auto const& entity : m_actable_entities)
		{
			//such as change of target

		}


		Entity camera_target = camera->get_target();
		if (camera_target == -1)
		{
			return;
		}
		auto target_sprite = gCoordinator.GetComponentReadOnly(camera_target, A::ComponentTypeEnum::RENDERABLE);

		if (target_sprite == nullptr)
		{
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Camera target destroyed.");
			camera_target = -1;
			return;
		}

		//Find the x/y of the target entity and update the camera position accordingly
		if (target_sprite->component_type == A::ComponentTypeEnum::RENDERABLE)
		{
			A::Renderable* target_sprite_renderable = (A::Renderable*) target_sprite;
			SDL_Rect position;
			position.x = target_sprite_renderable->x_;
			position.y = target_sprite_renderable->y_;
			position.w = target_sprite_renderable->tile_map_snip_.w;
			position.h = target_sprite_renderable->tile_map_snip_.h;

			camera->set_position_actual(position);
			SDL_Rect position_scaled = position;
			position_scaled.x *= Database::get_scale();
			position_scaled.y *= Database::get_scale();
			camera->set_position_scaled(position_scaled);
		}
	}
}