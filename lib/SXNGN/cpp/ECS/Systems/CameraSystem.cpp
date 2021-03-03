#include <ECS/Systems/CameraSystem.hpp>
#include <ECS/Components/Camera.hpp>
#include <ECS/Core/Coordinator.hpp>
#include <Database.h>
#include <ECS/Components/Renderable.hpp>



	void Camera_System::Init()
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Camera_System Init");
	}

	void Camera_System::Update(float dt)
	{

		auto camera = SXNGN::ECS::Components::Camera::get_instance();
		if (camera == nullptr)
		{
			return;
		}
		auto gCoordinator = *SXNGN::Database::get_coordinator();

		for (auto const& entity : m_actable_entities)
		{


		}
			

		Entity camera_target = camera->get_target();
		if (camera_target == -1)
		{
			return;
		}
		auto target_sprite = gCoordinator.GetComponent(camera_target, SXNGN::ECS::Components::ComponentTypeEnum::RENDERABLE);

		if (target_sprite == nullptr)
		{
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Camera target destroyed.");
			camera_target = -1;
			return;
		}

		if (target_sprite->get_component_type() == SXNGN::ECS::Components::ComponentTypeEnum::RENDERABLE)
		{
			SXNGN::ECS::Components::Renderable *target_sprite_renderable = (SXNGN::ECS::Components::Renderable*) target_sprite;
			
			camera->set_position_actual(*target_sprite_renderable->bounding_box_);




		}

		camera->set_position_actual(target_sprite->
		)









	}

