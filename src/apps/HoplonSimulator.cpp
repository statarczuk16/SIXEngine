/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, strings, and file streams
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include "../../lib/SXNGN/headers/gameutils.h"
#include "../../lib/SXNGN/headers/Tile.h"
#include "../../lib/SXNGN/headers/Collision.h"
#include "../../lib/SXNGN/headers/Texture.h"
#include "../../lib/SXNGN/headers/Constants.h"
#include "../../lib/SXNGN/headers/Object/Entity.h"

	//Screen dimension constants
	const int SCREEN_WIDTH = 640*2;
	const int SCREEN_HEIGHT = 480*2;

	//The dimensions of the level
	const int LEVEL_WIDTH = 14 * SXNGN::DEFAULT_TILE_WIDTH;
	const int LEVEL_HEIGHT = 5 * SXNGN::DEFAULT_TILE_HEIGHT;

	SDL_Rect g_level_bounds;

	SDL_Rect g_screen_bounds;


	std::string g_media_folder = SXNGN::BAD_STRING_RETURN;

	

	//Starts up SDL and creates window
	bool init();

	//Loads media
	bool loadMedia();

	//Frees media and shuts down SDL
	void close();



	//The window we'll be rendering to
	SDL_Window* gWindow = NULL;

	//The window renderer
	SDL_Renderer* gRenderer = NULL;

	//Scene textures
	std::shared_ptr<SXNGN::Texture> g_dot_texture_;

	std::shared_ptr<SXNGN::TileHandler> g_tile_handler_desert_map_;

	std::shared_ptr<SXNGN::TileHandler> g_tile_handler_apocalpyse_map_;

	std::vector<SXNGN::Tile> g_overworld_map_tiles_;
	

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
			//Set texture filtering to linear
			if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
			{
				printf("Warning: Linear texture filtering not enabled!");
			}

			//Create window
			gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
			}
		}

		g_level_bounds.x = 0;
		g_level_bounds.y = 0;
		g_level_bounds.w = LEVEL_WIDTH;
		g_level_bounds.h = LEVEL_HEIGHT;

		g_screen_bounds.y = 0;
		g_screen_bounds.x = 0;
		g_screen_bounds.w = SCREEN_WIDTH;
		g_screen_bounds.h = SCREEN_HEIGHT;
		return success;
	}

	bool loadMedia()
	{
		//Loading success flag
		bool success = true;
		g_media_folder = "uninit";
		g_media_folder = Gameutils::find_folder_in_project_string("media");
		if (g_media_folder == SXNGN::BAD_STRING_RETURN)
		{
			std::cout << "Warning: " << " Could not find media folder" << std::endl;
			success = false;
		}

		g_dot_texture_ = std::make_shared<SXNGN::Texture>(gRenderer);
		//Load dot texture
		if (!g_dot_texture_->loadFromFile(g_media_folder + "/entities/dot.bmp"))
		{
			printf("Failed to load dot texture!\n");
			success = false;
		}

		try
		{
			//fixme could get all this by reading a config file in the tile map folder
			std::string desert_tile_sheet_path = g_media_folder + "/desert_tile/tiles.png";
			std::string desert_tile_map_path = g_media_folder + "/desert_tile/map.map";
			std::string desert_tile_name_list_path = g_media_folder + "/desert_tile/tile_names.txt";
			std::string apoc_tile_map_folder_path_ = g_media_folder + "/wasteland_tile/";

			//set up a handler associates a list of "tile names" (list_path) with a sprite sheet (sheet_path)
			g_tile_handler_desert_map_ = std::make_shared<SXNGN::TileHandler>(gRenderer, desert_tile_sheet_path, desert_tile_map_path, desert_tile_name_list_path, 12, 192, SXNGN::DEFAULT_TILE_WIDTH, SXNGN::DEFAULT_TILE_HEIGHT);
			//load more 
			g_tile_handler_desert_map_->setTileNameTileType("BLACK", SXNGN::TileType::WALL); //all desert tile with name "BLACK" will be of TileType "WALL"
			success = success && g_tile_handler_desert_map_->loadTileMap(g_overworld_map_tiles_, g_level_bounds);//vector is pass by reference

			g_tile_handler_apocalpyse_map_ = std::make_shared<SXNGN::TileHandler>(gRenderer, apoc_tile_map_folder_path_);

		}
		catch (std::exception e)
		{
			printf("Failed to load tile map!\n");
			success = false;
		}


		return success;
	}

	void close()
	{
		//Destroy window	
		SDL_DestroyRenderer(gRenderer);//probably dont need to do this for shared ptr
		SDL_DestroyWindow(gWindow);
		gWindow = NULL;
		gRenderer = NULL;

		//Quit SDL subsystems
		IMG_Quit();
		SDL_Quit();
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

				//The dot that will be moving around on the screen
				//SXNGN::Dot dot;
				std::shared_ptr< SXNGN::Entity> gunman_entity;
				std::shared_ptr< SXNGN::Object> barrel_object;
				try
				{
					std::shared_ptr<SXNGN::Tile>gunman_tile =
						std::make_shared<SXNGN::Tile>(g_tile_handler_apocalpyse_map_->generateTile("GUNMAN_1"));
					gunman_entity = std::make_shared< SXNGN::Entity>(gunman_tile, 690);
					std::shared_ptr<SXNGN::Tile>barrel_tile =
						std::make_shared<SXNGN::Tile>(g_tile_handler_apocalpyse_map_->generateTile("BARREL"));
					barrel_object = std::make_shared< SXNGN::Object>(barrel_tile, 5);
				}
				catch (std::exception e)
				{
					printf("Failed to load tile map!\n");
					return 0;
				}
				

				//Level camera
				SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

				//While application is running
				while (!quit)
				{
					//Handle events on queue
					while (SDL_PollEvent(&e) != 0)
					{
						//User requests quit
						if (e.type == SDL_QUIT)
						{
							quit = true;
						}

						//Handle input for the dot
						gunman_entity->handleEvent(e);
						barrel_object->handleEvent(e);
					}

					//Move the dot
					gunman_entity->move(g_overworld_map_tiles_,g_level_bounds);
					barrel_object->setCamera(camera,g_level_bounds);

					gunman_entity->move(g_overworld_map_tiles_, g_level_bounds);

					barrel_object->move(g_overworld_map_tiles_, g_level_bounds);

					//Clear screen
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderClear(gRenderer);

					//Render level
					for(auto tile : g_overworld_map_tiles_)
					{
						tile.render(camera);
					}

					//Render dot
					gunman_entity->render(camera, g_dot_texture_);
					barrel_object->render(camera, g_dot_texture_);
					//Update screen
					SDL_RenderPresent(gRenderer);
				}
			}

			//Free resources and close SDL
			return 0;
		}

		