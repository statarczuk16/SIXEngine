
#include <State/BaseGameState.h>
const std::shared_ptr<SXNGN::Room>  SXNGN::BaseGameState::get_room()
{
	return room_;
}

SXNGN::BaseGameState::BaseGameState(std::shared_ptr<Room> room, std::shared_ptr<SXNGN::Object> entities)
{
	room_ = std::move(room);
	entities_ = entities;
}
