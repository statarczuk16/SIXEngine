#include <ECS/Components/UserInput.hpp>

namespace SXNGN::ECS::Components {
	//Have to define the singleton in a cpp
	User_Input_State* User_Input_State::instance_;
	std::mutex User_Input_State::lock_;
}