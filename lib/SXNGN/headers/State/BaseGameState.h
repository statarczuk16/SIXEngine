#pragma once

#include "World/Room.h"


namespace SXNGN {
	//The dot that will move around on the screen
	class BaseGameState
	{
	public:
		//Initializes the variables
		BaseGameState(std::shared_ptr<Room> room, std::shared_ptr<SXNGN::Object> entities);

		const std::shared_ptr<SXNGN::Room> get_room();

	protected:
		std::shared_ptr<Room> room_;
		std::shared_ptr<SXNGN::Object> entities_;
	};
}