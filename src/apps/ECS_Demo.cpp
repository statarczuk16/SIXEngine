#pragma warning(N:4596)


#include "ECS/Core/Coordinator.hpp"
#include "ECS/Systems/RenderSystem.hpp"
#include "ECS/Systems/MovementSystem.hpp"
#include "ECS/Systems/UserInputSystem.hpp"
#include <ECS/Components/Components.hpp>
#include <ECS/Utilities/Entity_Builder_Utils.hpp>
#include <ECS/Utilities/JSON_Utils.hpp>

#include <chrono>
#include <random>
#include <Database.h>
#include <gameutils.h>
//Using SDL, SDL_image, standard IO, strings, and file streams
#include <SDL.h>
#include <SDL_image.h>
#include<kiss_sdl.h>
#include <nlohmann/json.hpp>
#include <Timer.h>
#include <fstream>
#include <tuple>


SXNGN::ECS::Core::Coordinator gCoordinator;

static bool quit = false;

std::string g_media_folder = SXNGN::BAD_STRING_RETURN;//todo put in database?

std::string g_save_folder = SXNGN::BAD_STRING_RETURN;//todo put in database?

SDL_Renderer* gRenderer = NULL;

SDL_Rect g_level_bounds;

SDL_Rect g_screen_bounds;


const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 400;

const int LEVEL_WIDTH_PIXELS = 800;
const int LEVEL_HEIGHT_PIXELS = 600;

SXNGN::Database g_database; //static can be accessed anywhere, but must intialize


using Pre_Sprite_Factory = SXNGN::ECS::Components::Pre_Sprite_Factory;
using Sprite_Factory = SXNGN::ECS::Components::Sprite_Factory;
using Pre_Renderable = SXNGN::ECS::Components::Pre_Renderable;
using Renderable = SXNGN::ECS::Components::Renderable;
using Collisionable = SXNGN::ECS::Components::Collisionable;
using Tile = SXNGN::ECS::Components::Tile;
using Gameutils = SXNGN::Gameutils;
using nlohmann::json;
using entity_builder = SXNGN::ECS::Entity_Builder_Utils;
using Coordinator = SXNGN::ECS::Core::Coordinator;

using Movement_System = SXNGN::ECS::System::Movement_System;


void QuitHandler(Event& event)
{
	quit = true;
}



bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		g_level_bounds.x = 0;
		g_level_bounds.y = 0;
		g_level_bounds.w = LEVEL_WIDTH_PIXELS;
		g_level_bounds.h = LEVEL_HEIGHT_PIXELS;

		g_screen_bounds.x = 0;
		g_screen_bounds.y = 0;
		g_screen_bounds.w = SCREEN_WIDTH;
		g_screen_bounds.h = SCREEN_HEIGHT;

		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		
		g_media_folder = Gameutils::find_folder_in_project_string("media");
		if (g_media_folder == SXNGN::BAD_STRING_RETURN)
		{
			std::cout << "Fatal: " << " Could not find media folder" << std::endl;
			return 0;
		}

		g_save_folder = Gameutils::find_folder_in_project_string("save");
		if (g_save_folder == SXNGN::BAD_STRING_RETURN)
		{
			std::cout << "Fatal: " << " Could not find save folder" << std::endl;
			return 0;
		}


		std::string kiss_resource_folder = g_media_folder + "/kiss_resources/";
		if (Gameutils::file_exists(kiss_resource_folder + "/kiss_font.ttf"))
		{
			printf("Found KISS media folder %s\n", kiss_resource_folder.c_str());
		}
		else
		{
			printf("Fatal: Could not load kiss media folder\n");
			return 0;
		}
		kiss_array kiss_objects;
		gRenderer = kiss_init("HOPLON", &kiss_objects, g_screen_bounds.w, g_screen_bounds.h, kiss_resource_folder.c_str());
		//Create renderer for window
		//gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (gRenderer == NULL)
		{
			printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Initialize renderer color
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

			//Initialize PNG loading
			int imgFlags = IMG_INIT_PNG;
			if (!(IMG_Init(imgFlags) & imgFlags))
			{
				printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
				success = false;
			}
		}
		//}
	}

	SXNGN::Database::set_scale(1);// SXNGN::TILE_WIDTH_SCALE);

	return success;
}


int main(int argc, char* args[])
{
	if (!init())
	{
		printf("Fatal: Could not init\n");
		return 0;
	}
	gCoordinator.Init(gRenderer);
	SXNGN::Database::set_coordinator(std::make_shared<Coordinator>(gCoordinator));

	

	gCoordinator.AddEventListener(FUNCTION_LISTENER(Events::Window::QUIT, QuitHandler));


	gCoordinator.RegisterComponent(ComponentTypeEnum::SPRITE_FACTORY);
	gCoordinator.RegisterComponent(ComponentTypeEnum::PRE_SPRITE_FACTORY);
	gCoordinator.RegisterComponent(ComponentTypeEnum::PRE_RENDERABLE);
	gCoordinator.RegisterComponent(ComponentTypeEnum::RENDERABLE);
	gCoordinator.RegisterComponent(ComponentTypeEnum::CAMERA);
	gCoordinator.RegisterComponent(ComponentTypeEnum::INPUT_CACHE);
	gCoordinator.RegisterComponent(ComponentTypeEnum::INPUT_TAGS);
	gCoordinator.RegisterComponent(ComponentTypeEnum::MOVEABLE);
	gCoordinator.RegisterComponent(ComponentTypeEnum::COLLISION);
	gCoordinator.RegisterComponent(ComponentTypeEnum::TILE);


	

	//Sprite Factory Creator system looks for Pre_Sprite_Factories and uses them create Sprite_Factories
	auto sprite_factory_creator_system = gCoordinator.RegisterSystem<Sprite_Factory_Creator_System>();
	{
		Signature signature;
		signature.set(gCoordinator.GetComponentType(ComponentTypeEnum::PRE_SPRITE_FACTORY));
		gCoordinator.SetSystemSignatureActable<Sprite_Factory_Creator_System>(signature);
	}
	sprite_factory_creator_system->Init();

	//Sprite Factory Creator system looks for Pre_Renderables and creates Renderables
	//(Sprites that can be rendered to screen)
	auto sprite_factory_system = gCoordinator.RegisterSystem<Sprite_Factory_System>();
	{
		Signature signature_actable;
		signature_actable.set(gCoordinator.GetComponentType(ComponentTypeEnum::PRE_RENDERABLE));
		gCoordinator.SetSystemSignatureActable<Sprite_Factory_System>(signature_actable);
		Signature signature_of_interest;
		signature_of_interest.set(gCoordinator.GetComponentType(ComponentTypeEnum::SPRITE_FACTORY));
		gCoordinator.SetSystemSignatureOfInterest<Sprite_Factory_System>(signature_of_interest);
	}
	sprite_factory_system->Init();

	//Renderer takes Renderables and ... renders them
	auto renderer_system = gCoordinator.RegisterSystem<Renderer_System>();
	{
		Signature signature;
		signature.set(gCoordinator.GetComponentType(ComponentTypeEnum::RENDERABLE));
		gCoordinator.SetSystemSignatureActable<Renderer_System>(signature);
	}
	renderer_system->Init();


	auto input_system = gCoordinator.RegisterSystem<User_Input_System>();
	{
		//User_Input_System ACTS on any entity that has INPUT_CACHE (it USES input_cache)
		Signature signature_actable;
		signature_actable.set(gCoordinator.GetComponentType(ComponentTypeEnum::INPUT_CACHE));
		gCoordinator.SetSystemSignatureActable<User_Input_System>(signature_actable);
		//User_Input_System is INTERESTED in any entity that has INPUT_TAGS (It uses input_cache to apply input to entities according to their INPUT_TAGS)
		Signature signature_of_interest;
		signature_of_interest.set(gCoordinator.GetComponentType(ComponentTypeEnum::INPUT_TAGS));
		gCoordinator.SetSystemSignatureOfInterest<User_Input_System>(signature_of_interest);
	}
	input_system->Init();

	auto movement_system = gCoordinator.RegisterSystem<Movement_System>();
	{
		//User_Input_System ACTS on any entity that has MOVEABLE (it moves objects around)
		Signature signature_actable;
		signature_actable.set(gCoordinator.GetComponentType(ComponentTypeEnum::MOVEABLE));
		gCoordinator.SetSystemSignatureActable<Movement_System>(signature_actable);
		//User_Input_System is INTERESTED in any entity that has COLLISION (Checks collision data while moving actables)
		Signature signature_of_interest;
		signature_of_interest.set(gCoordinator.GetComponentType(ComponentTypeEnum::COLLISION));
		gCoordinator.SetSystemSignatureOfInterest<Movement_System>(signature_of_interest);
	}
	movement_system->Init();

	json j2 = {
  {"pi", 3.141},
  {"happy", true},
  {"name", "Niels"},
  {"nothing", nullptr},
  {"answer", {
	{"everything", 42}
  }},
  {"list", {1, 0, 2}},
  {"object", {
	{"currency", "USD"},
	{"value", 42.99}
  }}
	};

	std::cout << j2;


	SDL_Event e;

	//std::vector<Entity> entities(MAX_ENTITIES - 1);

	g_media_folder = "uninit";
	g_media_folder = Gameutils::find_folder_in_project_string("media");
	if (g_media_folder == SXNGN::BAD_STRING_RETURN)
	{
		std::cout << "Fatal: " << " Could not find media folder" << std::endl;
		return 0;
	}


	//Build the apocalypse map sprite factory
	auto apoc_map_pre_entity = gCoordinator.CreateEntity();
	SXNGN::ECS::Components::Pre_Sprite_Factory* apocalypse_map_pre = new Pre_Sprite_Factory();
	std::string apoc_tile_manifest_path = g_media_folder + "/wasteland_tile/manifest.txt";
	apocalypse_map_pre->name_ = "APOCALYPSE_MAP";
	apocalypse_map_pre->tile_manifest_path_ = apoc_tile_manifest_path;
	gCoordinator.AddComponent(apoc_map_pre_entity, apocalypse_map_pre);

	Entity test_person = entity_builder::Create_Person(gCoordinator, 0, 0, "APOCALYPSE_MAP", "GUNMAN_1", true);

	Entity test_Tile = entity_builder::Create_Tile(gCoordinator, 0, 0, "APOCALYPSE_MAP", "ROCK_GROUND", SXNGN::ECS::Components::CollisionType::NONE);

	//todo save utility or game state
	std::string savefile = g_save_folder + "/save1.json";
	std::ifstream ifs(savefile);
	json jf = json::parse(ifs);
	std::cout << jf << std::endl;

	std::tuple<std::vector<Pre_Renderable>, std::vector<Collisionable>, std::vector<Tile>> game_map;
	for (auto& [key, val] : jf.items())
	{
		std::cout << "key: " << key << ", value:" << val << '\n';
		if (key == "map")
		{
			game_map = SXNGN::ECS::Components::JSON_Utils::json_to_tile_batch(val);
		}
	}

	auto game_map_pre_renders = std::get<0>(game_map);
	auto game_map_collisionables = std::get<1>(game_map);
	auto game_map_tiles = std::get<2>(game_map);

	for (int i = 0; i < game_map_pre_renders.size(); i++)
	{
		auto map_tile_entity = gCoordinator.CreateEntity();
		Pre_Renderable* pre_render = new Pre_Renderable(game_map_pre_renders.at(i));
		gCoordinator.AddComponent(map_tile_entity, pre_render, true);
		Collisionable* collisionable = new Collisionable(game_map_collisionables.at(i));
		gCoordinator.AddComponent(map_tile_entity, collisionable, true);
		Tile* tile = new Tile(game_map_tiles.at(i));
		gCoordinator.AddComponent(map_tile_entity, tile, true);
	}

	




	//std::cout << extracted << std::endl;

	//auto json_to_entity_test = SXNGN::ECS::Components::JSON_Utils::json_to_component(extracted);
	/**
		json j = game_map_pre_renders;
		std::cout << j << std::endl;

	std::string savefile_out = g_save_folder + "/save2.json";
	std::ofstream file(savefile_out);
	file << j;
	**/

	SDL_Rect camera_lens;
	camera_lens.h = SCREEN_HEIGHT;
	camera_lens.w = SCREEN_WIDTH;
	camera_lens.x = 0;
	camera_lens.y = 0;

	SDL_Rect camera_position;
	camera_position.x = 0;
	camera_position.y = 0;
	camera_position.w = 0;
	camera_position.h = 0;

	SXNGN::ECS::Components::CameraComponent::init_instance(camera_lens, camera_position, g_screen_bounds);





	SXNGN::Timer dt_timer;//time passed during this frame "delta t"
	SXNGN::Timer frame_timer;//use to calculate frame rate
	SXNGN::Timer frame_cap_timer;//use to enforce frame cap
	frame_timer.start();

	float fps_avg = 0.0;
	float dt = 0.0;
	float game_dt;

	int frame_count = 0;
	std::vector<SDL_Event> events_this_frame;
	while (!quit)
	{
		/////////////////////////////////Frame Start
		frame_cap_timer.start();//this timer must reached ticks per frame before the next frame can start

		/////////////////////////////////Handle Game State
		//Todo game state
		/////////////////////////////////Event Handling
		events_this_frame.clear();
		while (SDL_PollEvent(&e) != 0)
		{
			events_this_frame.push_back(e);
			//queue up and add to event component type
		}
		//If any input occured, create an entity to carry them to the input_system
		if (!events_this_frame.empty())
		{
			auto input_event = gCoordinator.CreateEntity();
			SXNGN::ECS::Components::User_Input_Cache* input_cache = new User_Input_Cache();
			input_cache->events_ = events_this_frame;
			gCoordinator.AddComponent(input_event, input_cache, true);
			//update event handling system
			input_system->Update(dt);
		}



		/////////////////////////////////Physics/Movement
		//Phys start
		dt = dt_timer.getTicks() / 1000.f;//
		game_dt = dt;
		//todo - game_dt = 0.0 if paused - some systems don't operate when paused but some do
		movement_system->Update(dt);


		dt_timer.start(); //restart delta t for next frame
		//Phys End
		/////////////////////////////////Rendering
		//Render Setup
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);
		//System Related to Rendering but Don't Render
		sprite_factory_creator_system->Update(dt);
		sprite_factory_system->Update(dt);
		//Render UI
		//TODO add UI
		//Render Game
		renderer_system->Update(dt);

		auto stopTime = std::chrono::high_resolution_clock::now();

		//Render End
		SDL_RenderPresent(gRenderer);
		/////////////////////////////////Frame End
		fps_avg = frame_count / (frame_timer.getTicks() / 1000.f);
		if (fps_avg > 2000000)
		{
			fps_avg = 0;
		}
		if (frame_cap_timer.getTicks() < SXNGN::Database::get_screen_ticks_per_frame())
		{
			SDL_Delay(SXNGN::Database::get_screen_ticks_per_frame() - frame_cap_timer.getTicks());
		}
		++frame_count;

	}


	return 0;
}
