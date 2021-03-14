//Using SDL, SDL_image, standard IO, strings, and file streams
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include "../../lib/SXNGN/headers/gameutils.h"
#include <Sprite/Tile.h>
#include "../../lib/SXNGN/headers/Collision.h"
#include "../../lib/SXNGN/headers/Texture.h"
#include "../../lib/SXNGN/headers/Constants.h"
#include "../../lib/SXNGN/headers/Object/Entity.h"
#include <World/Room.h>
#include <State/BaseGameState.h>
#include <Database.h>
#include <Timer.h>
#include<kiss_sdl.h>

		//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 400;

const int LEVEL_WIDTH_PIXELS = 800;
const int LEVEL_HEIGHT_PIXELS = 600;

//The dimensions of the level
const int LEVEL_WIDTH_TILES = LEVEL_WIDTH_PIXELS / SXNGN::BASE_TILE_WIDTH;
const int LEVEL_HEIGHT_TILES = LEVEL_HEIGHT_PIXELS / SXNGN::BASE_TILE_HEIGHT;


SXNGN::Database g_database; //static can be accessed anywhere, but must intialize


SDL_Rect g_level_bounds;

SDL_Rect g_screen_bounds;


std::string g_media_folder = SXNGN::BAD_STRING_RETURN;

//some entities
		//PhysicsObj moves with acceleration
std::shared_ptr< SXNGN::PhysicsObj> gunman_entity;
//Object moves with constant velocity
std::shared_ptr< SXNGN::Object> barrel_object;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();



//The window we'll be rendering to
//SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures
std::shared_ptr<SXNGN::Texture> g_dot_texture_;

std::shared_ptr<SXNGN::TileHandler> g_tile_handler_desert_map_;

std::shared_ptr<SXNGN::TileHandler> g_tile_handler_apocalpyse_map_;

//std::vector<SXNGN::Tile> g_overworld_map_tiles_;

std::map<std::string, std::shared_ptr<SXNGN::BaseGameState>>  g_game_states_;

SXNGN::BaseGameState* g_current_state = nullptr;

std::string g_current_state_str = "uninit";


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

		//Create window
		//gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		//if (gWindow == NULL)
		//{
		//	printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		//	success = false;
		//}
		//else
		//{
			g_media_folder = "uninit";
			g_media_folder = SXNGN::Gameutils::find_folder_in_project_string("media");
			if (g_media_folder == SXNGN::BAD_STRING_RETURN)
			{
				std::cout << "Fatal: " << " Could not find media folder" << std::endl;
				return 0;
			}
			std::string kiss_resource_folder = g_media_folder + "/kiss_resources/";
			if (SXNGN::Gameutils::file_exists(kiss_resource_folder + "/kiss_font.ttf"))
			{
				printf("Found KISS media folder %s", kiss_resource_folder.c_str());
			}
			else
			{
				printf("Fatal: Could not load kiss media folder");
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



	SXNGN::Database::set_scale(SXNGN::TILE_WIDTH_SCALE);

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;
	

	g_dot_texture_ = std::make_shared<SXNGN::Texture>(gRenderer);
	//Load dot texture
	if (!g_dot_texture_->loadFromFile(g_media_folder + "/entities/dot.bmp"))
	{
		printf("Failed to load dot texture!\n");
		success = false;
	}

	try
	{

		std::string apoc_tile_map_folder_path_ = g_media_folder + "/wasteland_tile/";
		std::string desert_tile_map_path = g_media_folder + "/desert_tile/";
		std::string world_map_path = g_media_folder + "/maps/world_map.map";

		//set up a handler associates a list of "tile names" (list_path) with a sprite sheet (sheet_path)
		g_tile_handler_desert_map_ = std::make_shared<SXNGN::TileHandler>(gRenderer, desert_tile_map_path);
		//load more 
		g_tile_handler_desert_map_->setTileNameTileType("BLACK", SXNGN::TileType::WALL); //all desert tile with name "BLACK" will be of TileType "WALL"
		//success = success && g_tile_handler_desert_map_->loadTileMap(g_overworld_map_tiles_,world_map_path);//vector is pass by reference

		//Use a map file to generate a graphical map using the associations in the tile handler
		auto world_map = g_tile_handler_desert_map_->loadTileMap2D(world_map_path, success);
		if (!success)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error loading 2D Tile Map from: ", world_map_path);
			return false;
		}
		//create a "room" (wrapper for the graphical map)
		std::shared_ptr<SXNGN::Room> world_map_2D = std::make_shared<SXNGN::Room>(world_map);
		//create a game state out of the room TODO nullptr should be list of entities existing in the room
		std::shared_ptr<SXNGN::BaseGameState> game_state_hoplon_outside = std::make_shared<SXNGN::BaseGameState>(std::move(world_map_2D), nullptr);
		//move this game state to the global game states map
		g_game_states_["HOPLON_OUTSIDE"] = std::move(game_state_hoplon_outside);

		g_tile_handler_apocalpyse_map_ = std::make_shared<SXNGN::TileHandler>(gRenderer, apoc_tile_map_folder_path_);

	}
	catch (std::exception e)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load tile map with error: %s", e.what());
		success = false;
	}
	return success;
}

void close()
{
	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	//SDL_DestroyWindow(gWindow);
	//gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

bool switch_state(std::string new_state)
{
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "switching state: from: %s to: %s",
		g_current_state_str.c_str(), new_state.c_str());


	if (g_game_states_.count(new_state) != 1)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "switching state: no such state in map: %s",
			new_state.c_str());
		return false;
	}
	g_current_state = g_game_states_.at(new_state).get();

	if (g_current_state == nullptr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "switching state: state was null: %s",
			new_state.c_str());
		return false;
	}
	return true;
}

static void button_sw_event(kiss_button* button, SDL_Event* e, int* draw)
{
	std::shared_ptr<SXNGN::Tile>mailman_tile =
		g_tile_handler_apocalpyse_map_->generateTileRef("MAILMAN");
	if (kiss_button_event(button, e, draw))
	{
		gunman_entity->get_sprite_ref()->getTileClipBox()->x = mailman_tile->getTileClipBox()->x;
		gunman_entity->get_sprite_ref()->getTileClipBox()->y = mailman_tile->getTileClipBox()->y;
	}
}

static void button_sw2_event(kiss_button* button, SDL_Event* e, int* draw)
{
	std::shared_ptr<SXNGN::Tile>mailman_tile =
		g_tile_handler_apocalpyse_map_->generateTileRef("ROCK_GROUND");
	if (kiss_button_event(button, e, draw))
	{
		gunman_entity->get_sprite_ref()->getTileClipBox()->x = mailman_tile->getTileClipBox()->x;
		gunman_entity->get_sprite_ref()->getTileClipBox()->y = mailman_tile->getTileClipBox()->y;
	}
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{

		if (!loadMedia())
		{
			printf("Failed to initialize!\n");
			return 0;
		}
		//Fixme initialize a STATE
		//Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event e;
		bool change_state = true;

		g_current_state_str = "HOPLON_OUTSIDE";


		

		std::shared_ptr<SXNGN::Tile>gunman_tile =
			g_tile_handler_apocalpyse_map_->generateTileRef("GUNMAN_1");
		if (gunman_tile)
		{
			gunman_entity = std::make_shared< SXNGN::PhysicsObj>(gunman_tile, 3000);
		}
		else
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create from sprite name GUNMAN_1");
			return 0;
		}
		std::shared_ptr<SXNGN::Tile>barrel_tile =
			g_tile_handler_apocalpyse_map_->generateTileRef("BARREL");
		if (barrel_tile)
		{
			barrel_object = std::make_shared< SXNGN::Object>(barrel_tile, 5);
		}
		else
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create from sprite name BARREL");
			return 0;
		}




		//Create a camera 
		SDL_Rect camera_lens;
		camera_lens.h = SCREEN_HEIGHT;
		camera_lens.w = SCREEN_WIDTH;
		camera_lens.x = 0;
		camera_lens.y = 0;

		SDL_Rect camera_position;
		camera_position.x = 0;
		camera_position.y = 0;

		std::shared_ptr<SXNGN::Camera> main_cam_ = std::make_shared<SXNGN::Camera>(camera_lens, camera_position, g_screen_bounds);

		//Assign the camera to track this sprite
		main_cam_->lock_on(gunman_entity->get_sprite_ref());

		SXNGN::Timer move_timer;
		SXNGN::Timer frame_timer;
		SXNGN::Timer frame_cap_timer;
		frame_timer.start();

		int frame_count = 0;

		kiss_window window_low;
		kiss_button button_switch = { 0 };
		kiss_button button_switch_2 = { 0 };

		kiss_window_new(&window_low, NULL, 1, 0, g_screen_bounds.h-60, g_screen_bounds.w,60);
		kiss_button_new_uc(&button_switch, &window_low, "Switch", 20, g_screen_bounds.h - 50, 0);
		kiss_button_new_uc(&button_switch_2, &window_low, "ASDFG", 25, g_screen_bounds.h - 50, 0);


		window_low.visible = 1;
		int draw_ui = 1;
		//While application is running
		while (!quit)
		{

			frame_cap_timer.start();//this timer must reached ticks per frame before the next frame can start
			if (change_state)
			{
				switch_state(g_current_state_str);
				if (g_current_state == nullptr)
				{
					printf("Failed start state\n");
					return 0;
				}
				change_state = false;
			}
			///////////Event Handling
			while (SDL_PollEvent(&e) != 0)
			{
				//User requests quit
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
				//On mouse wheel scroll
				if (e.type == SDL_EventType::SDL_MOUSEWHEEL)
				{
				
					if(e.wheel.y < 0)
					{
						SXNGN::Database::reduce_scale();
					}
					else
					{
						SXNGN::Database::increase_scale();
					}
					
				}

				//Handle input for the dot
				gunman_entity->handleEvent(e);
				barrel_object->handleEvent(e);

				button_sw_event(&button_switch, &e, &draw_ui);
				button_sw2_event(&button_switch_2, &e, &draw_ui);
			}

			////////////Physics
			float fps_avg = frame_count / (frame_timer.getTicks() / 1000.f);
			if (fps_avg > 2000000)
			{
				fps_avg = 0;
			}
			//printf("%g\n", fps_avg);
			

			float time_step_seconds = move_timer.getTicks() / 1000.f;

			//Dont bother with physics unless positive frame rate to avoid divide by zero erros
			if(fps_avg>0 && time_step_seconds > 0)
			{
			
				
				gunman_entity->move(g_current_state->get_room()->get_tiles_1D(), g_current_state->get_room()->get_room_bounds(), time_step_seconds);
				main_cam_->move(time_step_seconds);
			}
			
			//barrel_object->setCamera(camera,g_level_bounds);

			//barrel_object->move(g_overworld_map_tiles_, g_level_bounds);

			move_timer.start();
			///////////////Rendering Game
			//Clear screen
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear(gRenderer);

			//Render level
			for (auto tile : g_current_state->get_room()->get_tiles_1D())
			{
				tile.render(main_cam_);
			}

			//Render dot
			gunman_entity->render(main_cam_);
			//barrel_object->render(main_cam_);
			//Update screen

			///////////////Rendering UI
			kiss_window_draw(&window_low, gRenderer);
			kiss_button_draw(&button_switch, gRenderer);
			kiss_button_draw(&button_switch_2, gRenderer);

			SDL_RenderPresent(gRenderer);
			//delay if frame finished early (so maintain capped frames per second)
			if (frame_cap_timer.getTicks() < SXNGN::Database::get_screen_ticks_per_frame())
			{
				SDL_Delay(SXNGN::Database::get_screen_ticks_per_frame() - frame_cap_timer.getTicks());
			}
			++frame_count;
		}
	}

	//Free resources and close SDL
	return 0;
}

