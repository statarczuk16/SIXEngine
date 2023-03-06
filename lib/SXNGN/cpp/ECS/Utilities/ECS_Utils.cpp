#include <ECS/Utilities/ECS_Utils.hpp>
#include <Collision.h>

namespace SXNGN {
	namespace ECS {

			

			bool ECS_Utils::object_in_view(std::shared_ptr<ECS_Camera> camera, SDL_FRect object_bounds)
			{
				//Have to scale up object to compare to the camera
				object_bounds.x *= SXNGN::Database::get_scale();
				object_bounds.y *= SXNGN::Database::get_scale();
				object_bounds.w *= SXNGN::Database::get_scale();
				object_bounds.h *= SXNGN::Database::get_scale();
				SDL_FRect scaled_camera_lens = determine_camera_lens_scaled(camera);

				//Note object_bounds.w * 2 -- extra buffer so objects a bit out of camera range are still rendered
				//dont want stuff to disappear//appear while the player can still see them
				if (SXNGN::CollisionChecks::checkCollisionRectRectBuffer(scaled_camera_lens, object_bounds, object_bounds.w * 2))
				{
					return true;
				}
				return false;
			}

			SDL_FRect ECS_Utils::determine_camera_lens_scaled(std::shared_ptr<ECS_Camera> camera)
			{
				SDL_FRect return_view;
				SDL_FRect position_actual = camera->position_actual_;
				SDL_FRect position_scaled;
				double scale_ = SXNGN::Database::get_scale();
				position_scaled.x = position_actual.x * SXNGN::Database::get_scale();
				position_scaled.y = position_actual.y * SXNGN::Database::get_scale();
				camera->position_scaled_ = position_scaled;
				SDL_FRect screen_bounds = camera->screen_bounds_;
				SDL_FRect lens = camera->lens_;

				//bounding box centers are at the top left of the box.
				//If the camera is tracking a target at (10,10), then the top left of the camera vision square should not be the target
				//instead the top left vision square of the camera should be some ways left and some ways above the tracked target
				//such that a tracked target is in the center of the view (rather than being at the top left)
				return_view.x = position_scaled.x - (lens.w / 2);
				return_view.y = position_scaled.y - (lens.h / 2);
				return_view.w = lens.w;
				return_view.h = lens.h;

				SDL_FRect screen_bounds_scaled = screen_bounds;

				screen_bounds_scaled.x *= SXNGN::Database::get_scale();
				screen_bounds_scaled.y *= SXNGN::Database::get_scale();
				screen_bounds_scaled.w *= SXNGN::Database::get_scale();
				screen_bounds_scaled.h *= SXNGN::Database::get_scale();
				//don't allow left side of camera to go further left than the screen
				
				if (camera->bounded_horizontal_)
				{
					
					//don't right side of camera further right than right side of screen
					if ((return_view.x + lens.w) > (screen_bounds_scaled.x + screen_bounds_scaled.w))
					{
						return_view.x = (screen_bounds_scaled.x + screen_bounds_scaled.w - lens.w);
					}
					if (return_view.x < screen_bounds_scaled.x)
					{
						return_view.x = screen_bounds_scaled.x;
					}
					
				}
			
				if(camera->bounded_vertical_)
				{
					if (return_view.y < screen_bounds_scaled.y)
					{
						return_view.y = screen_bounds_scaled.y;
					}
					if (return_view.y + lens.h > (screen_bounds_scaled.y + screen_bounds_scaled.h))
					{
						return_view.y = screen_bounds_scaled.y + screen_bounds_scaled.h - lens.h;
					}
				}
				
				return return_view;
			}

			SDL_FRect ECS_Utils::determine_camera_lens_unscaled(std::shared_ptr<ECS_Camera> camera)
			{
				SDL_FRect current_view = determine_camera_lens_scaled(camera);
				current_view.x /= SXNGN::Database::get_scale();
				current_view.y /= SXNGN::Database::get_scale();
				return current_view;
			}

			SDL_FRect ECS_Utils::convert_screen_position_to_world_position(std::shared_ptr<ECS_Camera> camera, int screen_x, int screen_y, ComponentTypeEnum game_state)
			{
				SDL_FRect world_position;
				//account for camera offset
				SDL_FRect current_view = determine_camera_lens_scaled(camera);
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
				int temp_x = (int)round(world_position.x / SXNGN::Database::get_scale());
				int temp_y = (int)round(world_position.y / SXNGN::Database::get_scale());
				world_position.x = temp_x;
				world_position.y = temp_y;
				
				
				return world_position;

			}

			void ECS_Utils::pause_game()
			{
				auto gCoordinator = SXNGN::Database::get_coordinator();
				auto pause_game_event_pair = gCoordinator->getEvent(SXNGN::PAUSE);
				if (pause_game_event_pair.second)
				{
					for (auto func : pause_game_event_pair.first.e.func_event.callbacks)
					{
						func();
					}
				}
			}

			void ECS_Utils::unpause_game()
			{
				auto gCoordinator = SXNGN::Database::get_coordinator();
				auto pause_game_event_pair = gCoordinator->getEvent(SXNGN::UNPAUSE);
				if (pause_game_event_pair.second)
				{
					for (auto func : pause_game_event_pair.first.e.func_event.callbacks)
					{
						func();
					}
				}
			}
		}
	}