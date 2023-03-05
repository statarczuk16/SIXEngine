
#include <ECS/Components/CameraComponent.hpp>
#include <ECS/Core/Coordinator.hpp>
#include <Database.h>
#include <ECS/Components/Renderable.hpp>
#include <ECS/Systems/CameraSystem.hpp>

namespace SXNGN::ECS {

	void Camera_System::Init()
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Camera_System Init");
	}

	/// <summary>
	/// Camera's goals: Don't do much, just follow around the target.
	/// </summary>
	/// <param name="dt"></param>
	void Camera_System::Update(double dt)
	{

		auto camera = CameraComponent::get_instance();
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
		auto target_location = gCoordinator.GetComponentReadOnly(camera_target, ComponentTypeEnum::LOCATION);

		if (target_location == nullptr)
		{
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Camera target destroyed.");
			camera_target = -1;
			return;
		}

		//Find the x/y of the target entity and update the camera position accordingly
		if (target_location->component_type == ComponentTypeEnum::LOCATION)
		{
			Location* target_location_ptr = (Location*) target_location;
			Coordinate coordinate = target_location_ptr->GetPixelCoordinate();
			SDL_FRect position;
			position.x = coordinate.x;
			position.y = coordinate.y;
			position.w = 0;// = target_location_ptr->tile_map_snip_.w;
			position.h = 0;// target_location_ptr->tile_map_snip_.h;

			camera->set_position_actual(position);
			SDL_FRect position_scaled = position;
			position_scaled.x *= Database::get_scale();
			position_scaled.y *= Database::get_scale();
			camera->set_position_scaled(position_scaled);
		}
	}
}