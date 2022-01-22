#include <ECS/Utilities/ECS_Utils.hpp>
#include <Collision.h>

namespace SXNGN {
	namespace ECS {
		namespace A {


			std::shared_ptr<SDL_Rect> ECS_Utils::GetEntityPosition(Entity entity)
			{
				auto gCoordinator = *SXNGN::Database::get_coordinator();
				if (gCoordinator.EntityHasComponent(entity, ComponentTypeEnum::MOVEABLE))
				{
					auto moveable_data = gCoordinator.GetComponentReadOnly(entity, ComponentTypeEnum::MOVEABLE);
					if (moveable_data)
					{
						const Moveable* moveable = static_cast<const Moveable*>(moveable_data);
						std::shared_ptr<SDL_Rect> pos = std::make_shared<SDL_Rect>();
						pos->x = (int) round(moveable->m_pos_x_m);
						pos->y = (int) round(moveable->m_pos_y_m);
						return pos;

					}
				}
				return nullptr;
			 }

			void ECS_Utils::ChangeEntityPositionLastGood(Entity entity)
			{
				auto gCoordinator = *SXNGN::Database::get_coordinator();

				if (gCoordinator.EntityHasComponent(entity, ComponentTypeEnum::MOVEABLE))
				{
					auto moveable_data = gCoordinator.CheckOutComponent(entity, ComponentTypeEnum::MOVEABLE);
					if (moveable_data)
					{
						Moveable* moveable = static_cast<Moveable*>(moveable_data);
						auto last_x = moveable->m_prev_pos_x_m;
						auto last_y = moveable->m_prev_pos_y_m;

						gCoordinator.CheckInComponent(ComponentTypeEnum::MOVEABLE, entity);

						ChangeEntityPosition(entity, last_x, last_y, true);
					}
				}
			}

			void ECS_Utils::ChangeEntityPosition(Entity entity, double x, double y, bool confident)
			{
				
				auto gCoordinator = *SXNGN::Database::get_coordinator();

				if (gCoordinator.EntityHasComponent(entity, ComponentTypeEnum::MOVEABLE))
				{
					auto moveable_data = gCoordinator.CheckOutComponent(entity, ComponentTypeEnum::MOVEABLE);
					if (moveable_data)
					{
						Moveable* moveable = static_cast<Moveable*>(moveable_data);
						if (moveable->m_pos_x_m == x && moveable->m_pos_y_m == y)
						{
							gCoordinator.CheckInComponent(ComponentTypeEnum::MOVEABLE, entity);
							return;
						}
						if (!confident)
						{
							moveable->m_prev_pos_x_m = moveable->m_pos_x_m;
							moveable->m_prev_pos_y_m = moveable->m_pos_y_m;
						}
						
						moveable->UpdatePosition(x, y);
						gCoordinator.CheckInComponent(ComponentTypeEnum::MOVEABLE, entity);
					}
				}

				if (gCoordinator.EntityHasComponent(entity, ComponentTypeEnum::RENDERABLE))
				{
					auto moveable_renderbox = gCoordinator.CheckOutComponent(entity, ComponentTypeEnum::RENDERABLE);
					if (moveable_renderbox)
					{
						Renderable* render_ptr = static_cast<Renderable*>(moveable_renderbox);
						render_ptr->x_ = int(round(x));
						render_ptr->y_ = int(round(y));
						gCoordinator.CheckInComponent(ComponentTypeEnum::RENDERABLE, entity);
					}
				}

				if (gCoordinator.EntityHasComponent(entity, ComponentTypeEnum::COLLISION))
				{
					auto collisionable_data = gCoordinator.CheckOutComponent(entity, ComponentTypeEnum::COLLISION);
					if (collisionable_data)
					{
						Collisionable* collision_ptr = static_cast<Collisionable*>(collisionable_data);
						collision_ptr->collision_box_.x = int(round(x));
						collision_ptr->collision_box_.y = int(round(y));
						collision_ptr->resolved_ = false;
						gCoordinator.CheckInComponent(ComponentTypeEnum::COLLISION, entity);
					}
				}

				

			}

			bool ECS_Utils::object_in_view(std::shared_ptr<ECS_Camera> camera, SDL_Rect object_bounds)
			{
				//Have to scale up object to compare to the camera
				object_bounds.x *= SXNGN::Database::get_scale();
				object_bounds.y *= SXNGN::Database::get_scale();
				object_bounds.w *= SXNGN::Database::get_scale();
				object_bounds.h *= SXNGN::Database::get_scale();
				SDL_Rect scaled_camera_lens = determine_camera_lens_scaled(camera);

				//Note object_bounds.w * 2 -- extra buffer so objects a bit out of camera range are still rendered
				//dont want stuff to disappear//appear while the player can still see them
				if (SXNGN::CollisionChecks::checkCollisionBuffer(scaled_camera_lens, object_bounds, object_bounds.w * 2))
				{
					return true;
				}
				return false;
			}

			SDL_Rect ECS_Utils::determine_camera_lens_scaled(std::shared_ptr<ECS_Camera> camera)
			{
				SDL_Rect return_view;
				SDL_Rect position_actual = camera->position_actual_;
				SDL_Rect position_scaled;
				position_scaled.x = position_actual.x * SXNGN::Database::get_scale();
				position_scaled.y = position_actual.y * SXNGN::Database::get_scale();
				camera->position_scaled_ = position_scaled;
				SDL_Rect screen_bounds = camera->screen_bounds_;
				SDL_Rect lens = camera->lens_;

				//bounding box centers are at the top left of the box.
				//If the camera is tracking a target at (10,10), then the top left of the camera vision square should not be the target
				//instead the top left vision square of the camera should be some ways left and some ways above the tracked target
				//such that a tracked target is in the center of the view (rather than being at the top left)
				return_view.x = position_scaled.x - (lens.w / 2);
				return_view.y = position_scaled.y - (lens.h / 2);
				return_view.w = lens.w;
				return_view.h = lens.h;

				SDL_Rect screen_bounds_scaled = screen_bounds;

				screen_bounds_scaled.x *= SXNGN::Database::get_scale();
				screen_bounds_scaled.y *= SXNGN::Database::get_scale();
				screen_bounds_scaled.w *= SXNGN::Database::get_scale();
				screen_bounds_scaled.h *= SXNGN::Database::get_scale();


				//don't allow camera top-right view square point to leave screen
				if (return_view.x < screen_bounds_scaled.x)
				{
					return_view.x = screen_bounds_scaled.x;
				}
				if (return_view.x > (screen_bounds_scaled.x + screen_bounds_scaled.w))
				{
					return_view.x = screen_bounds_scaled.x + screen_bounds_scaled.w;
				}

				if (return_view.y < screen_bounds_scaled.y)
				{
					return_view.y = screen_bounds_scaled.y;
				}
				if (return_view.y > (screen_bounds_scaled.y + screen_bounds_scaled.h))
				{
					return_view.y = screen_bounds_scaled.y + screen_bounds_scaled.h;
				}

				return return_view;
			}

			SDL_Rect ECS_Utils::determine_camera_lens_unscaled(std::shared_ptr<ECS_Camera> camera)
			{
				SDL_Rect current_view = determine_camera_lens_scaled(camera);
				current_view.x /= SXNGN::Database::get_scale();
				current_view.y /= SXNGN::Database::get_scale();
				return current_view;
			}

			SDL_Rect ECS_Utils::convert_screen_position_to_world_position(std::shared_ptr<ECS_Camera> camera, int screen_x, int screen_y, ComponentTypeEnum game_state)
			{
				SDL_Rect world_position;
				//account for camera offset
				SDL_Rect current_view = determine_camera_lens_unscaled(camera);
				world_position.x = current_view.x + screen_x;
				world_position.y = current_view.y + screen_y;
				world_position.w = 0;
				world_position.h = 0;
				auto gCoordinator = SXNGN::Database::get_coordinator();
				std::shared_ptr<SDL_Rect> overworld_viewport = gCoordinator->get_state_manager()->getStateViewPort(game_state);
				if (overworld_viewport == nullptr)
				{
					return world_position;
				}
				//account for viewport offset
				world_position.x -= overworld_viewport->x;
				world_position.y -= overworld_viewport->y;
				
				return world_position;

			}
		}
	}
}