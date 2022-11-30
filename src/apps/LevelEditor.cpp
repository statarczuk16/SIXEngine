//Using SDL, SDL_image, standard IO, strings, and file streams
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <gameutils.h>
#include <Sprite/Tile.h>
#include <Collision.h>
#include <Texture.h>
#include <Constants.h>
#include <Object/Entity.h>
#include <Object/MouseObj.h>
#include <Camera.h>
#include "ECS/Core/Coordinator.hpp"

	//Screen dimension constants
	const int SCREEN_WIDTH = 640*2;
	const int SCREEN_HEIGHT = 480*2;

	const int LEVEL_WIDTH_PIXELS = 400;
	const int LEVEL_HEIGHT_PIXELS = 200;


	//The dimensions of the level
	const int LEVEL_WIDTH_TILES = LEVEL_WIDTH_PIXELS / SXNGN::BASE_TILE_WIDTH;
	const int LEVEL_HEIGHT_TILES = LEVEL_HEIGHT_PIXELS / SXNGN::BASE_TILE_HEIGHT;


	

	const int LEVEL_WIDTH = SXNGN::BASE_TILE_WIDTH * LEVEL_WIDTH_TILES;
	const int LEVEL_HEIGHT = SXNGN::BASE_TILE_HEIGHT * LEVEL_HEIGHT_TILES;

	SDL_Rect g_level_bounds;

	SDL_Rect g_screen_bounds;

	SXNGN::Tile g_test_tile;


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

	std::shared_ptr<SXNGN::TileHandler> g_tile_handler_apocalpyse_map_;

	std::shared_ptr < SXNGN::Texture> g_blank_texture_;

	std::vector< std::vector<std::shared_ptr<SXNGN::Tile>> > g_map_tiles_; 

	std::vector< std::vector<std::shared_ptr<SXNGN::Tile>> > g_map_tiles_background_;

	

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
			

			//Create window
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
		g_media_folder = SXNGN::Gameutils::find_folder_in_project_string("media");
		if (g_media_folder == SXNGN::BAD_STRING_RETURN)
		{
			std::cout << "Warning: " << " Could not find media folder" << std::endl;
			success = false;
		}

		try
		{
			std::string apoc_tile_map_folder_path_ = g_media_folder + "/wasteland_tile/";
			g_tile_handler_apocalpyse_map_ = std::make_shared<SXNGN::TileHandler>(gRenderer, apoc_tile_map_folder_path_);
			g_test_tile = g_tile_handler_apocalpyse_map_->generateTile("GUNMAN_2");

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

	void place_tile(SDL_Rect& camera, std::string tile_type)
	{
		//Mouse offsets
		int mouse_x = 0, mouse_y = 0;

		//Get mouse offsets
		SDL_GetMouseState(&mouse_x, &mouse_y);

		//Adjust to camera
		mouse_x += camera.x;
		mouse_y += camera.y;

		mouse_y = mouse_y / SXNGN::TILE_HEIGHT_SCALE;
		mouse_x = mouse_x / SXNGN::TILE_WIDTH_SCALE;

		for (int map_x = 0; map_x < LEVEL_WIDTH_TILES; map_x++)
		{
			
			for (int map_y = 0; map_y < LEVEL_HEIGHT_TILES; map_y++)
			{
				auto collision_box = g_map_tiles_.at(map_x).at(map_y)->getCollisionBox();
				if ((mouse_x > collision_box->x) && (mouse_x < collision_box->x + collision_box->w)
					&& (mouse_y > collision_box->y) && (mouse_y < collision_box->y + collision_box->h))
				{
					auto tile = g_tile_handler_apocalpyse_map_->generateTileRef(tile_type);
					tile->getCollisionBox()->h = SXNGN::BASE_TILE_HEIGHT;
					tile->getCollisionBox()->w = SXNGN::BASE_TILE_WIDTH;
					tile->getCollisionBox()->x = map_x * SXNGN::BASE_TILE_WIDTH;;
					tile->getCollisionBox()->y = map_y * SXNGN::BASE_TILE_HEIGHT;
					g_map_tiles_.at(map_x).at(map_y) = tile;
				}
			}
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

				std::shared_ptr<SXNGN::MouseObject> mouse = std::make_shared < SXNGN::MouseObject>();
				

				g_blank_texture_ = std::make_shared<SXNGN::Texture>(gRenderer);
				

				for (int w = 0; w < LEVEL_WIDTH_TILES; w++)
				{
					std::vector<std::shared_ptr<SXNGN::Tile>> row;
					for (int h = 0; h < LEVEL_HEIGHT_TILES; h++)
					{
						std::shared_ptr<SXNGN::Tile> blank_tile_inst  =  std::make_shared<SXNGN::Tile>(g_blank_texture_);
						blank_tile_inst->getCollisionBox()->h = SXNGN::BASE_TILE_HEIGHT;
						blank_tile_inst->getCollisionBox()->w = SXNGN::BASE_TILE_WIDTH;
						blank_tile_inst->getCollisionBox()->x = w * SXNGN::BASE_TILE_WIDTH;
						blank_tile_inst->getCollisionBox()->y = h * SXNGN::BASE_TILE_HEIGHT;
						row.push_back((blank_tile_inst));
						printf("[%2d,%2d]", w,h);
					}
					g_map_tiles_.push_back((row));
					printf("\n \n \n");
				}

				for (int w = 0; w < LEVEL_WIDTH_TILES; w++)
				{
					std::vector<std::shared_ptr<SXNGN::Tile>> row;
					for (int h = 0; h < LEVEL_HEIGHT_TILES; h++)
					{
						std::shared_ptr<SXNGN::Tile> blank_tile_inst = std::make_shared<SXNGN::Tile>(g_blank_texture_);
						blank_tile_inst->getCollisionBox()->h = SXNGN::BASE_TILE_HEIGHT;
						blank_tile_inst->getCollisionBox()->w = SXNGN::BASE_TILE_WIDTH;
						blank_tile_inst->getCollisionBox()->x = w * SXNGN::BASE_TILE_WIDTH;
						blank_tile_inst->getCollisionBox()->y = h * SXNGN::BASE_TILE_HEIGHT;
						row.push_back((blank_tile_inst));
						printf("[%2d,%2d]", w, h);
					}
					g_map_tiles_background_.push_back((row));
					printf("\n \n \n");
				}

				//Level camera
				SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

				SDL_Rect camera_lens;
				camera_lens.h = SCREEN_HEIGHT;
				camera_lens.w = SCREEN_WIDTH;
				camera_lens.x = 0;
				camera_lens.y = 0;

				SDL_Rect camera_position;
				camera_position.x = 0;
				camera_position.y = 0;

				std::shared_ptr<SXNGN::Camera> main_cam_ = std::make_shared<SXNGN::Camera>(camera_lens, camera_position, g_screen_bounds);


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
						//When the user clicks
						if (e.type == SDL_MOUSEBUTTONDOWN)
						{
							//On left mouse click
							if (e.button.button == SDL_BUTTON_LEFT)
							{
								//Put the tile
								place_tile(camera, "GUNMAN_2");
							}
						}
						
					}

					mouse->move();
					mouse->setCamera(camera, g_screen_bounds);

					//Clear screen
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderClear(gRenderer);

					//Render level

					for (auto tile_map : g_map_tiles_background_)
					{
						for (auto tile : tile_map)
						{
							tile->render(main_cam_);
						}
					}

					for(auto tile_map : g_map_tiles_)
					{
						for (auto tile : tile_map)
						{
							tile->render(main_cam_);
						}
					}

					//Render dot
					//Update screen
					SDL_RenderPresent(gRenderer);

				}
			}

			//Free resources and close SDL
			return 0;
		}

		