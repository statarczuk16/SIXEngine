#include <Object/Object.h>
#include <Texture.h>
#include <Tile.h>
#include <Physics.h>


SXNGN::Object::Object(std::shared_ptr< SXNGN::Tile> tile, int speed) : entity_tile_(tile)
{
	//Initialize the collision box
	collision_box_.x = 0;
	collision_box_.y = 0;
	collision_box_.w = tile->getTileClipBox()->w;
	collision_box_.h = tile->getTileClipBox()->h;
	m_speed_m_s = speed;
	entity_tile_ = tile;

	name_ = tile->getTileName();
}

void SXNGN::Object::Object::handleEvent(SDL_Event& e)
{
	//If a key was pressed
	//If a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: m_vel_y_m_s -= m_speed_m_s; break;
		case SDLK_DOWN: m_vel_y_m_s += m_speed_m_s; break;
		case SDLK_LEFT: m_vel_x_m_s -= m_speed_m_s; break;
		case SDLK_RIGHT: m_vel_x_m_s += m_speed_m_s; break;
		}
	}
	//If a key was released
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: m_vel_y_m_s += m_speed_m_s; break;
		case SDLK_DOWN: m_vel_y_m_s -= m_speed_m_s; break;
		case SDLK_LEFT: m_vel_x_m_s += m_speed_m_s; break;
		case SDLK_RIGHT: m_vel_x_m_s -= m_speed_m_s; break;
		}
	}
}

void SXNGN::Object::move(std::vector<SXNGN::Tile> tiles, SDL_Rect level_bounds, float time_step)
{
	SDL_Rect prev_box = collision_box_;
	int x_bound_max = level_bounds.x + level_bounds.w;
	int x_bound_min = level_bounds.x;
	int y_bound_max = level_bounds.y + level_bounds.h;
	int y_bound_min = level_bounds.y;
	//Move the dot left or right
	collision_box_.x += int(m_vel_x_m_s * time_step);

	//If the dot went too far to the left or right or touched a wall
	if ((collision_box_.x < x_bound_min) || (collision_box_.x + collision_box_.w > x_bound_max) || SXNGN::Collision::touchesWall(collision_box_, tiles))
	{
		//move back
		collision_box_.x = prev_box.x;
	}

	//Move the dot up or down
	collision_box_.y += int(m_vel_y_m_s * time_step);

	//If the dot went too far up or down or touched a wall
	if ((collision_box_.y < y_bound_min) || (collision_box_.y + collision_box_.h > y_bound_max) || SXNGN::Collision::touchesWall(collision_box_, tiles))
	{
		//move back
		collision_box_.y = prev_box.y;
	}
}


void SXNGN::Object::setCamera(SDL_Rect& camera, SDL_Rect screen_bounds)
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

void SXNGN::Object::render(SDL_Rect& camera, std::shared_ptr<SXNGN::Texture> dot_texture)
{
	//Show the dot
	entity_tile_->getCollisionBox()->x = collision_box_.x;
	entity_tile_->getCollisionBox()->y = collision_box_.y;
	entity_tile_->getCollisionBox()->w = collision_box_.w;
	entity_tile_->getCollisionBox()->h = collision_box_.h;
	entity_tile_->render(camera);
}