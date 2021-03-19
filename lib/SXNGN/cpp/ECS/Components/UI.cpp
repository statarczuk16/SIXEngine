#include <ECS/Components/UI.hpp>

namespace SXNGN::ECS::A {
	//Have to define the singleton in a cpp
	std::shared_ptr<UICollectionSingleton> UICollectionSingleton::instance_;
	std::mutex UICollectionSingleton::lock_;
}