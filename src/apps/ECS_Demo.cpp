#pragma warning(N:4596)

#include "ECS/Components/Renderable.hpp"
#include "ECS/Core/Coordinator.hpp"
#include "ECS/Systems/RenderSystem.hpp"
#include <chrono>
#include <random>
#include <Database.h>
#include <gameutils.h>
//Using SDL, SDL_image, standard IO, strings, and file streams
#include <SDL.h>
#include <SDL_image.h>
#include<kiss_sdl.h>
#include <ECS/Components/Camera.hpp>
#include <Timer.h>

Coordinator gCoordinator;

static bool quit = false;

std::string g_media_folder = SXNGN::BAD_STRING_RETURN;//todo put in database?

SDL_Renderer* gRenderer = NULL;

SDL_Rect g_level_bounds;

SDL_Rect g_screen_bounds;


const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 400;

const int LEVEL_WIDTH_PIXELS = 800;
const int LEVEL_HEIGHT_PIXELS = 600;



using Pre_Sprite_Factory = SXNGN::ECS::Components::Pre_Sprite_Factory;
using Sprite_Factory = SXNGN::ECS::Components::Sprite_Factory;
using Pre_Renderable = SXNGN::ECS::Components::Pre_Renderable;
using Renderable = SXNGN::ECS::Components::Renderable;


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

		g_media_folder = "uninit";
		g_media_folder = Gameutils::find_folder_in_project_string("media");
		if (g_media_folder == SXNGN::BAD_STRING_RETURN)
		{
			std::cout << "Fatal: " << " Could not find media folder" << std::endl;
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
	//renderer_system->Init();


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

	//Build a tile to render
	auto pre_renderable_test_entity = gCoordinator.CreateEntity();
	SXNGN::ECS::Components::Pre_Renderable* pre_renderable = new Pre_Renderable();
	pre_renderable->sprite_factory_name = "APOCALYPSE_MAP";
	pre_renderable->sprite_factory_sprite_type = "GUNMAN_1";
	gCoordinator.AddComponent(pre_renderable_test_entity, pre_renderable);


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

	SXNGN::ECS::Components::Camera::init_instance(camera_lens, camera_position, g_screen_bounds);




	float dt = 0.0f;
	SXNGN::Timer dt_timer;//time passed during this frame "delta t"
	SXNGN::Timer frame_timer;//use to calculate frame rate
	SXNGN::Timer frame_cap_timer;//use to enforce frame cap
	frame_timer.start();

	float fps_avg = 0.0;
	float dt = 0.0;
	float game_dt;

	int frame_count = 0;
	while (!quit)
	{
		/////////////////////////////////Frame Start 
		frame_cap_timer.start();//this timer must reached ticks per frame before the next frame can start
		
								/////////Event Handling
		while (SDL_PollEvent(&e) != 0)
		{
			//TODO queue up and add to event component type
		}
		//Todo update event handling system


		/////////////////////////////////Physics/Movement
		//Phys start
		dt = dt_timer.getTicks() / 1000.f;//
		game_dt = dt;
		//todo - game_dt = 0.0 if paused - some systems don't operate when paused but some do
		//todo movement system


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
