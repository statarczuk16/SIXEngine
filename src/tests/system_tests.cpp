#include <sole.hpp>
#include <SDL.h>
#include<kiss_sdl.h>
#include <assert.h>

#include "ECS/Core/Coordinator.hpp"
#include <ECS/Components/Components.hpp>
#include <ECS/Utilities/Entity_Builder_Utils.hpp>
using nlohmann::json;

const int SCREEN_WIDTH = 640 * 2;
const int SCREEN_HEIGHT = 480 * 2;

int main(int argc, char* args[])
{
	bool success = true;
	SDL_Window* gWindow = NULL;
	SDL_Renderer* gRenderer = NULL;
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	gWindow = SDL_CreateWindow("Level Editor", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == NULL)
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Create renderer for window
		gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (gRenderer == NULL)
		{
			printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
	}

	SXNGN::ECS::A::Coordinator gCoordinator;
	gCoordinator.InitStateManager();
	gCoordinator.Init(gRenderer);


	SXNGN::Database::set_coordinator(std::make_shared<SXNGN::ECS::A::Coordinator>(gCoordinator));

	auto settings = gCoordinator.get_state_manager()->getGameSettings();
	gCoordinator.RegisterComponent(ComponentTypeEnum::COLLISION);
	gCoordinator.RegisterComponent(ComponentTypeEnum::TILE);
	int w = 5;
	int h = 10;
	Sint32 x_pixels = w * SXNGN::BASE_TILE_WIDTH;
	Sint32 y_pixels = h * SXNGN::BASE_TILE_HEIGHT;
	SDL_Rect collision_box;
	collision_box.x = x_pixels;
	collision_box.y = y_pixels;
	collision_box.w = SXNGN::BASE_TILE_WIDTH;
	collision_box.h = SXNGN::BASE_TILE_HEIGHT;
	SXNGN::ECS::A::Collisionable* collision;
	//SXNGN::ECS::A::Tile* tile;
	
	//collision = SXNGN::ECS::A::Entity_Builder_Utils::Create_Collisionable(collision_box, SXNGN::ECS::A::CollisionType::IMMOVEABLE);
	//tile = SXNGN::ECS::A::Entity_Builder_Utils::Create_Tile(w, h);

	auto map_tile_entity_1 = gCoordinator.CreateEntity();
	gCoordinator.AddComponent(map_tile_entity_1, collision);

	auto map_tile_entity_2 = gCoordinator.CreateEntity();
	gCoordinator.AddComponent(map_tile_entity_2, collision);


	sole::uuid uuid_1 = gCoordinator.GetUUIDFromEntity(map_tile_entity_1);
	assert(uuid_1 != SXNGN::BAD_UUID);

	sole::uuid uuid_2 = gCoordinator.GetUUIDFromEntity(map_tile_entity_2);
	assert(uuid_2 != SXNGN::BAD_UUID);

	return 0;

}