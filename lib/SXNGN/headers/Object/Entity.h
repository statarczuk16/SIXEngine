#pragma once

#include "../../headers/Object/Object.h"
#include "../../headers/Object/Entity.h"
#include "../../headers/Tile.h"
#include "../../headers/Collision.h"
#include <vector>
#include <SDL.h>
#include "../../headers/gameutils.h"

namespace SXNGN {
	//The dot that will move around on the screen
	class Entity : public Object
	{
	public:
		//Initializes the variables
		Entity(std::shared_ptr<Tile> tile, int speed);

		virtual void handleEvent(SDL_Event& e);

		//Moves the dot and check collision against tiles
		virtual void move(std::vector<SXNGN::Tile> tiles, SDL_Rect bounds, float time_step = 1);


	protected:
		//The velocity of the dot
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
