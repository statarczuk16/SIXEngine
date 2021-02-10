#include <Dot.h>
#include <Texture.h>


SXNGN::Dot::Dot()
{
	//Initialize the collision box
	m_box_.x = 0;
	m_box_.y = 0;
	m_box_.w = DOT_WIDTH;
	m_box_.h = DOT_HEIGHT;

	//Initialize the velocity
	mVelX = 0;
	mVelY = 0;
}

void SXNGN::Dot::handleEvent(SDL_Event& e)
{
	//If a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: mVelY -= DOT_VEL; break;
		case SDLK_DOWN: mVelY += DOT_VEL; break;
		case SDLK_LEFT: mVelX -= DOT_VEL; break;
		case SDLK_RIGHT: mVelX += DOT_VEL; break;
		}
	}
	//If a key was released
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: mVelY += DOT_VEL; break;
		case SDLK_DOWN: mVelY -= DOT_VEL; break;
		case SDLK_LEFT: mVelX += DOT_VEL; break;
		case SDLK_RIGHT: mVelX -= DOT_VEL; break;
		}
	}
}

void SXNGN::Dot::move(std::vector<SXNGN::Tile> tiles, SDL_Rect level_bounds, float time_step)
{

	SDL_Rect prev_box = m_box_;
		int x_bound_max = level_bounds.x + level_bounds.w;
		int x_bound_min = level_bounds.x;
		int y_bound_max = level_bounds.y + level_bounds.h;
		int y_bound_min = level_bounds.y;
		//Move the dot left or right
		m_box_.x += mVelX * time_step;

		//If the dot went too far to the left or right or touched a wall
		if ((m_box_.x < x_bound_min) || (m_box_.x + DOT_WIDTH > x_bound_max) || SXNGN::Collision::touchesWall(m_box_, tiles))
		{
			//move back
			m_box_.x = prev_box.x;
		}

		//Move the dot up or down
		m_box_.y += mVelY;

		//If the dot went too far up or down or touched a wall
		if ((m_box_.y < y_bound_min) || (m_box_.y + DOT_HEIGHT > y_bound_max) || SXNGN::Collision::touchesWall(m_box_, tiles))
		{
			//move back
			m_box_.y = prev_box.y;
		}
}




void SXNGN::Dot::setCamera(SDL_Rect& camera, SDL_Rect screen_bounds)
{
	//Center the camera over the dot
	camera.x = (m_box_.x + DOT_WIDTH / 2) - screen_bounds.w / 2;
	camera.y = (m_box_.y + DOT_HEIGHT / 2) - screen_bounds.h / 2;

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

void SXNGN::Dot::render(SDL_Rect& camera, std::shared_ptr<SXNGN::Texture> dot_texture)
{
	//Show the dot
	dot_texture->render(m_box_.x - camera.x, m_box_.y - camera.y);
}