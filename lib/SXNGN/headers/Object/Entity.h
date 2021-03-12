#pragma once

#include "../../headers/Object/Object.h"
#include "../../headers/Object/Entity.h"
#include <Sprite/Tile.h>
#include "../../headers/Collision.h"
#include <vector>
#include <SDL.h>
#include "../../headers/gameutils.h"

namespace SXNGN {
	class PhysicsObj : public Object
	{
	public:
		//Initializes the variables
		PhysicsObj(std::shared_ptr<Tile> tile, int speed);

		virtual void handleEvent(SDL_Event& e);

		//Moves the dot and check collision against tiles
		virtual void move(std::vector<SXNGN::Tile> tiles, SDL_Rect bounds, float time_step = 1);


	protected:

		//double position. With high enough frame rate, movement per frame may not be greater
		//than one (one pixel), so will need to accumulate over time before rounding to int.
		double phys_x, phys_y = 0.0; 
		double phys_m_vel_x_m_s_, phys_m_vel_y_m_s_ = 0.0;//velocity in meters / second in directions
		double m_acc_x_m_s_s_, m_acc_y_m_s_s_ = 0.0;//current acceleration m /s second squared
		double m_max_vel_m_s_ = 0.0;//maximum speed in any direction
		int speed_x_n_ = 0;//how fast is this entity - how much force can it impart on its own power
		int weight_kg_ = 0;
		//force applied to object
		int f_app_n_x_ = 0;
		int f_app_n_y_ = 0;
	};
}
