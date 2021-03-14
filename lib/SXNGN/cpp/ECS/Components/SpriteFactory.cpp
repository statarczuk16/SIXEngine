#include <ECS/Components/SpriteFactory.hpp>
#include <mutex>

namespace SXNGN::ECS::A {
	//Have to define the singleton in a cpp
	SpriteFactoryHolder* SpriteFactoryHolder::instance_;
	std::mutex SpriteFactoryHolder::lock_;
}