#include <Object/Entity.h>
#include <Texture.h>
#include <Tile.h>
#include <Physics.h>


SXNGN::Entity::Entity(std::shared_ptr< SXNGN::Tile> tile, int speed) : Object(tile, speed)
{
	//Initialize the collision box
	collision_box_.x = 0;
	collision_box_.y = 0;
	collision_box_.w = tile->getTileClipBox()->w;
	collision_box_.h = tile->getTileClipBox()->h;

	//Initialize the velocity
	phys_m_vel_x_m_s_ = 0;
	phys_m_vel_y_m_s_ = 0;
	m_acc_x_m_s_s_ = 0;
	m_acc_y_m_s_s_ = 0;
	m_max_vel_m_s_ = 3;
	speed_x_n_ = speed;
	weight_kg_ = 75;

	f_app_n_x_ = 0;
	f_app_n_y_ = 0;

	entity_tile_ = tile;

	name_ = tile->getTileName();
}

void SXNGN::Entity::Entity::handleEvent(SDL_Event& e)
{
	//If a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: f_app_n_y_ += -speed_x_n_; break;
		case SDLK_DOWN: f_app_n_y_ += speed_x_n_; break;
		case SDLK_LEFT: f_app_n_x_ += -speed_x_n_; break;
		case SDLK_RIGHT: f_app_n_x_ += speed_x_n_; break;
		}
	}

	if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: f_app_n_y_ -= -speed_x_n_; break;
		case SDLK_DOWN: f_app_n_y_ -= speed_x_n_; break;
		case SDLK_LEFT: f_app_n_x_ -= -speed_x_n_; break;
		case SDLK_RIGHT: f_app_n_x_ -= speed_x_n_; break;
		}
	} 
}

void SXNGN::Entity::move(std::vector<SXNGN::Tile> tiles, SDL_Rect level_bounds, float time_step)
{

		
		SDL_Rect prev_box = collision_box_;
		int x_bound_max = level_bounds.x + level_bounds.w;
		int x_bound_min = level_bounds.x;
		int y_bound_max = level_bounds.y + level_bounds.h;
		int y_bound_min = level_bounds.y;
		
		double x_friction_acc_m_s_s = 0.0;
		double y_friction_acc_m_s_s = 0.0;

		double x_app_acc_m_s_s = 0.0;
		double y_app_acc_m_s_s = 0.0;

		double vel_cached = phys_m_vel_x_m_s_ + phys_m_vel_y_m_s_;
		double force_cached = f_app_n_x_ + f_app_n_y_;
		double acc_cached = m_acc_x_m_s_s_ + m_acc_y_m_s_s_;




		
		x_friction_acc_m_s_s = SXNGN::Physics::get_friction_acc(weight_kg_, 0.7, phys_m_vel_x_m_s_, f_app_n_x_);
		y_friction_acc_m_s_s = SXNGN::Physics::get_friction_acc(weight_kg_, 0.7, phys_m_vel_y_m_s_, f_app_n_y_);



		
		//Accelerate due to force applied
		x_app_acc_m_s_s = SXNGN::Physics::get_applied_force_acc(double(f_app_n_x_), weight_kg_);
		y_app_acc_m_s_s = SXNGN::Physics::get_applied_force_acc(double(f_app_n_y_), weight_kg_);


		//Friction cannot propel you - if friction stronger than force applied, simply do not accelerate
		if (abs(x_friction_acc_m_s_s) > abs(x_app_acc_m_s_s) && phys_m_vel_x_m_s_ == 0.0)
		{
			m_acc_x_m_s_s_ = 0.0;
		}
		else
		{
			m_acc_x_m_s_s_ = (x_app_acc_m_s_s + x_friction_acc_m_s_s);
		}

		if (abs(y_friction_acc_m_s_s) > abs(y_app_acc_m_s_s) && phys_m_vel_y_m_s_ == 0.0)
		{
			m_acc_y_m_s_s_ = 0.0;
		}
		else
		{
			m_acc_y_m_s_s_ = (y_app_acc_m_s_s + y_friction_acc_m_s_s);
		}


		//move according to current acceleration
		phys_m_vel_x_m_s_ = Physics::adjust_velocity_by_acc(m_acc_x_m_s_s_, phys_m_vel_x_m_s_, time_step, m_max_vel_m_s_);
		phys_m_vel_y_m_s_ = Physics::adjust_velocity_by_acc(m_acc_y_m_s_s_, phys_m_vel_y_m_s_, time_step, m_max_vel_m_s_);


		if ((phys_m_vel_x_m_s_ + phys_m_vel_y_m_s_) != vel_cached || (f_app_n_x_ + f_app_n_y_) != force_cached || (m_acc_x_m_s_s_ + m_acc_y_m_s_s_) != acc_cached)
		{
			std::cout << "Entity: " << name_ << " movement: " << " force: " << f_app_n_x_ << " vel: " << phys_m_vel_x_m_s_ << " acc " << m_acc_x_m_s_s_ 
				<< "( " << x_app_acc_m_s_s << " + " << x_friction_acc_m_s_s << ") " << std::endl;
		}
		


		collision_box_.x += int(round((phys_m_vel_x_m_s_ * time_step)));

		//If the dot went too far to the left or right or touched a wall
		if ((collision_box_.x < x_bound_min) || (collision_box_.x + collision_box_.w > x_bound_max) || SXNGN::Collision::touchesWall(collision_box_, tiles))
		{
			//move back
			collision_box_.x = prev_box.x;
			phys_m_vel_x_m_s_ = 0.0;
			m_acc_x_m_s_s_ = 0.0;

		}

		//Move the dot up or down
		collision_box_.y += int(round((phys_m_vel_y_m_s_ * time_step)));

		//If the dot went too far up or down or touched a wall
		if ((collision_box_.y < y_bound_min) || (collision_box_.y + collision_box_.h > y_bound_max) || SXNGN::Collision::touchesWall(collision_box_, tiles))
		{
			//move back
			collision_box_.y = prev_box.y;
			phys_m_vel_y_m_s_ = 0.0;
			m_acc_y_m_s_s_ = 0.0;
		}
}

