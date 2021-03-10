#pragma once

#include <ECS/Core/Coordinator.hpp>
#include <Constants.h>
#include <filesystem>
#include <ECS/Components/Components.hpp>
#include <nlohmann/json.hpp>

namespace SXNGN {
	namespace ECS {
		class JSON_Utils
		{
		public:
			static std::tuple<std::vector<Components::Pre_Renderable>, std::vector<Components::Collisionable>, std::vector<Components::Tile>>
				json_to_tile_batch(nlohmann::json);
		};
	}
}