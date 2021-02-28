#include <Object/MouseObj.h>
#include <Texture.h>
#include <Sprite/Tile.h>
#include <Physics.h>
#include <Constants.h>


SXNGN::MouseObject::MouseObject(std::shared_ptr< SXNGN::Tile> tile)
{
	if (tile != nullptr)
	{

	}
	//Initialize the collision box
	collision_box_.x = 0;
	collision_box_.y = 0;
	if (tile != nullptr)
	{
		name_ = tile->getTileName();
		collision_box_.w = tile->getTileClipBox()->w;
		collision_box_.h = tile->getTileClipBox()->h;
	}
	else
	{
		name_ = "Mouse";
		collision_box_.w = SXNGN::BASE_TILE_WIDTH;
		collision_box_.h = SXNGN::BASE_TILE_HEIGHT;

	}
	
	sprite_ = tile;

}

void SXNGN::MouseObject::MouseObject::handleEvent(SDL_Event& e)
{
	
}

void SXNGN::MouseObject::move()
{
	//Mouse offsets
	int x = 0, y = 0;
	//Get mouse offsets
	SDL_GetMouseState(&x, &y);
	collision_box_.x = x;
	collision_box_.y = y;
}


void SXNGN::MouseObject::setCamera(SDL_Rect& camera, SDL_Rect screen_bounds)
{
	//Center the camera over the dot
	camera.x = (collision_box_.x + collision_box_.w / 2) - screen_bounds.w / 2;
	camera.y = (collision_box_.y + collision_box_.h / 2) - screen_bounds.h / 2;

	//Keep the camera in bounds
	if (camera.x < 0)
	{
		camera.x = 0;
	}
	if (camera.y < 0)
	{
		camera.y = 0;
	}
	if (camera.x > screen_bounds.w - camera.w)
	{
		camera.x = screen_bounds.w - camera.w;
	}
	if (camera.y > screen_bounds.h - camera.h)
	{
		camera.y = screen_bounds.h - camera.h;
	}
}

void SXNGN::MouseObject::render(std::shared_ptr<SXNGN::Camera> camera)
{
	//Show the dot
	if (sprite_ != nullptr)
	{
		sprite_->getCollisionBox()->x = collision_box_.x;
		sprite_->getCollisionBox()->y = collision_box_.y;
		sprite_->getCollisionBox()->w = collision_box_.w;
		sprite_->getCollisionBox()->h = collision_box_.h;
		sprite_->render(camera);
	}

}