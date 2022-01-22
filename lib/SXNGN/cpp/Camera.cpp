#include <Camera.h>
#include <Collision.h>
#include <Sprite/Tile.h>
#include <Database.h>


SXNGN::Camera::Camera(SDL_Rect lens, SDL_Rect position, SDL_Rect screen_bounds)
{
	lens_ = lens;
	position_actual_ = position;
	position_scaled_.x = position_actual_.x * SXNGN::Database::get_scale();
	position_scaled_.y = position_actual_.y * SXNGN::Database::get_scale();
	screen_bounds_ = screen_bounds;
}

void SXNGN::Camera::lock_on(std::shared_ptr<SXNGN::Tile> target)
{
	if (target != nullptr)
	{
		target_ = target;
	}
}

void SXNGN::Camera::move()
{
	if (target_ != nullptr)
	{
		position_actual_.x = target_->getCollisionBox()->x;
		position_actual_.y = target_->getCollisionBox()->y;
		position_scaled_.x = position_actual_.x * SXNGN::Database::get_scale();
		position_scaled_.y = position_actual_.y * SXNGN::Database::get_scale();
	}
}

bool SXNGN::Camera::object_in_view(SDL_Rect obj_bounds)
{
	//Have to scale up object to compare to the camera
	obj_bounds.x *= SXNGN::Database::get_scale();
	obj_bounds.y *= SXNGN::Database::get_scale();
	obj_bounds.w *= SXNGN::Database::get_scale();

	if (SXNGN::CollisionChecks::checkCollisionBuffer(get_lens_rect_scaled(), obj_bounds, obj_bounds.w*2))
	{
		return true;
	}
	return false;
}


SDL_Rect SXNGN::Camera::get_lens_rect_actual()
{
	SDL_Rect current_view = get_lens_rect_scaled();
	current_view.x /= SXNGN::Database::get_scale();
	current_view.y /= SXNGN::Database::get_scale();
	return current_view;
}


SDL_Rect SXNGN::Camera::get_lens_rect_scaled()
{
	SDL_Rect return_view;
	//bounding box centers are at the top left of the box.
	//If the camera is tracking a target at (10,10), then the top left of the camera vision square should not be the target
	//instead the top left vision square of the camera should be some ways left and some ways above the tracked target
	//such that a tracked target is in the center of the view (rather than being at the top left)
	return_view.x = position_scaled_.x - (lens_.w / 2);
	return_view.y = position_scaled_.y - (lens_.h / 2);
	return_view.w = lens_.w;
	return_view.h = lens_.h;

	SDL_Rect screen_bounds_scaled = screen_bounds_;

	screen_bounds_scaled.x *=  SXNGN::Database::get_scale();
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



