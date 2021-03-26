#pragma warning(N:4596)


#include "ECS/Core/Coordinator.hpp"
#include "ECS/Systems/RenderSystem.hpp"
#include "ECS/Systems/MovementSystem.hpp"
#include "ECS/Systems/UserInputSystem.hpp"
#include <ECS/Components/Components.hpp>
#include <ECS/Utilities/Entity_Builder_Utils.hpp>
#include <ECS/Utilities/JSON_Utils.hpp>
#include <ECS/Utilities/Map_Utils.hpp>

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
#include <ECS/Systems/EventSystem.hpp>


SXNGN::ECS::A::Coordinator gCoordinator;

static bool quit = false;

std::string g_media_folder = SXNGN::BAD_STRING_RETURN;//todo put in database?

std::string g_save_folder = SXNGN::BAD_STRING_RETURN;//todo put in database?

SDL_Renderer* gRenderer = NULL;

SDL_Rect g_level_bounds;

SDL_Rect g_screen_bounds;


const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

const int LEVEL_WIDTH_PIXELS = 800;
const int LEVEL_HEIGHT_PIXELS = 600;

SXNGN::Database g_database; //static can be accessed anywhere, but must intialize



using Gameutils = SXNGN::Gameutils;
using nlohmann::json;
using entity_builder = SXNGN::ECS::A::Entity_Builder_Utils;


using Movement_System = SXNGN::ECS::A::Movement_System;

using namespace SXNGN::ECS::A;

void QuitHandler(Event& event)
{
	quit = true;
}



bool init()
{
	//Initialization flag
	bool success = true;
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
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
		if (Gameutils::file_exists(kiss_resource_folder + "/manifest_do_not_delete.txt"))
		{
			printf("Found KISS media folder %s\n", kiss_resource_folder.c_str());
		}
		else
		{
			printf("Fatal: Could not load kiss media folder\n");
			return 0;
		}
		kiss_array kiss_objects;
		std::string tile_set = "sand";
		gRenderer = kiss_init("HOPLON", &kiss_objects, g_screen_bounds.w, g_screen_bounds.h, kiss_resource_folder.c_str(), tile_set.c_str());
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


int init_menus()
{
	auto coordinator = SXNGN::Database::get_coordinator();
	const int WINDOW_HEIGHT = 620;
	const int WINDOW_WIDTH = 320;
	auto ui = UICollectionSingleton::get_instance();

	//************************* Main Menu
	auto mmw_c = UserInputUtils::create_window_raw(nullptr, 220, g_screen_bounds.h - WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT, UILayer::BOTTOM);
	ui->add_ui_element(ComponentTypeEnum::MAIN_MENU_STATE, mmw_c);

	auto start_button_c = UserInputUtils::create_button(mmw_c->window_, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "New Game", 1);
	Event_Component new_game_event;
	new_game_event.e.common.type = EventType::STATE_CHANGE;
	new_game_event.e.state_change.new_states.push_front(ComponentTypeEnum::NEW_GAME_STATE);
	new_game_event.e.state_change.states_to_remove.push_front(ComponentTypeEnum::MAIN_MENU_STATE);
	start_button_c->triggered_events.push_back(new_game_event);
	mmw_c->child_components_.push_back(start_button_c);

	auto load_button_c = UserInputUtils::create_button(mmw_c->window_, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "New Game", 2);
	Event_Component load_game_event;
	load_game_event.e.common.type = EventType::LOAD;
	load_game_event.e.load.filePath = "No File Path";
	load_button_c->triggered_events.push_back(load_game_event);
	mmw_c->child_components_.push_back(load_button_c);

	auto settings_button_c = UserInputUtils::create_button(mmw_c->window_, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "Settings Game", 3);
	Event_Component settings_state_event;
	settings_state_event.e.common.type = EventType::STATE_CHANGE;
	settings_state_event.e.state_change.new_states.push_front(ComponentTypeEnum::MAIN_SETTINGS_STATE);
	settings_state_event.e.state_change.states_to_remove.push_front(ComponentTypeEnum::MAIN_MENU_STATE);
	settings_button_c->triggered_events.push_back(settings_state_event);
	settings_button_c->triggered_events.push_back(settings_state_event);
	mmw_c->child_components_.push_back(settings_button_c);

	auto exit_button_c = UserInputUtils::create_button(mmw_c->window_, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "Exit", 4);
	Event_Component exit_button_event;
	exit_button_event.e.common.type = EventType::EXIT;
	exit_button_c->triggered_events.push_back(exit_button_event);
	mmw_c->child_components_.push_back(exit_button_c);


	//************************* Settings Menu

	auto msw_container = UserInputUtils::create_window_raw(nullptr, 220, g_screen_bounds.h - WINDOW_HEIGHT, WINDOW_WIDTH*2, WINDOW_HEIGHT, UILayer::BOTTOM);
	ui->add_ui_element(ComponentTypeEnum::MAIN_SETTINGS_STATE, msw_container);

	const int NUM_SETTINGS = 9;
	for (int i = 0; i < NUM_SETTINGS; i++)
	{
		auto h_align = HA_COLUMN;
		auto v_align = VA_ROW;
		int row = i / 2;
		int column = i % 2;
		auto parent_scale = SP_HALF;
		auto setting_button_c_row = UserInputUtils::create_button(msw_container->window_, h_align, v_align, parent_scale, UILayer::MID, (char*)("Setting " + std::to_string(i)).c_str(), row, column);
		msw_container->child_components_.push_back(setting_button_c_row);
	}

	auto back_button_c = UserInputUtils::create_button(msw_container->window_, HA_CENTER, VA_ROW, SP_HALF, UILayer::MID, "Back", NUM_SETTINGS);
	
	Event_Component back_state_event;
	back_state_event.e.common.type = EventType::STATE_CHANGE;
	back_state_event.e.state_change.new_states.push_front(ComponentTypeEnum::MAIN_MENU_STATE);
	back_state_event.e.state_change.states_to_remove.push_front(ComponentTypeEnum::MAIN_SETTINGS_STATE);
	back_button_c->triggered_events.push_back(back_state_event);
	back_button_c->triggered_events.push_back(back_state_event);
	msw_container->child_components_.push_back(back_button_c);

	//************************* New Game Menu

	const int NUM_SETTINGS_NG = 9;
	auto new_game_window_c = UserInputUtils::create_window_raw(nullptr, 220, g_screen_bounds.h - WINDOW_HEIGHT, WINDOW_WIDTH * 2, WINDOW_HEIGHT, UILayer::BOTTOM);
	ui->add_ui_element(ComponentTypeEnum::NEW_GAME_STATE, new_game_window_c);

	//New Game Label
	auto ngl_c = UserInputUtils::create_label(new_game_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_THIRD, UILayer::MID, "New Game", 0);
	new_game_window_c->child_components_.push_back(ngl_c);

	auto lwel_c = UserInputUtils::create_label(new_game_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_THIRD, UILayer::MID, "Level Width (Chunks)", 1, 1);
	new_game_window_c->child_components_.push_back(lwel_c);

	auto lwe_c = UserInputUtils::create_num_entry(new_game_window_c->window_, HA_COLUMN, VA_ROW, SP_THIRD, UILayer::MID, "level_width_entry", 2, 24, TE_INT, 4, 2, 1);
	new_game_window_c->child_components_.push_back(lwe_c);

	auto lhel_c = UserInputUtils::create_label(new_game_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_THIRD, UILayer::MID, "Level Height (Chunks)", 3, 1);
	new_game_window_c->child_components_.push_back(lhel_c);

	auto lhe_c = UserInputUtils::create_num_entry(new_game_window_c->window_, HA_COLUMN, VA_ROW, SP_THIRD, UILayer::MID, "level_height_entry", 2, 24, TE_INT, 4, 4, 1);
	new_game_window_c->child_components_.push_back(lhe_c);

	
	//Callback functions for level width/height 
	std::function<void(std::shared_ptr<UIContainerComponent> uicc)> ng_update_width = [coordinator](std::shared_ptr<UIContainerComponent> uicc)
	{
		coordinator->get_state_manager()->setLevelWidthTiles(uicc->entry_->num_val);
	};

	std::function<void()> ng_update_width_b = std::bind(ng_update_width, lwe_c);
	lwe_c->callback_functions_.push_back(ng_update_width_b);

	//Callback functions for level width/height 
	std::function<void(std::shared_ptr<UIContainerComponent> uicc)> ng_update_height = [coordinator](std::shared_ptr<UIContainerComponent> uicc)
	{
		coordinator->get_state_manager()->setLevelHeightTiles(uicc->entry_->num_val);
	};

	std::function<void()> ng_update_height_b = std::bind(ng_update_height, lhe_c);
	lhe_c->callback_functions_.push_back(ng_update_height_b);
		
	auto new_game_start_b = UserInputUtils::create_button(new_game_window_c->window_, HA_CENTER, VA_ROW, SP_HALF, UILayer::MID, "Start", NUM_SETTINGS_NG-1);
	//Callback functions for level width/height 
	/**std::function<void(std::shared_ptr<UIContainerComponent> uicc)> ng_start_game = [coordinator](std::shared_ptr<UIContainerComponent> uicc)
	{
		
	};**/

	std::function<void()> ng_start = Map_Utils::StartNewGame;
	new_game_start_b->callback_functions_.push_back(ng_start);
	new_game_window_c->child_components_.push_back(new_game_start_b);
		

	auto back_from_bg_button_c = UserInputUtils::create_button(new_game_window_c->window_, HA_CENTER, VA_ROW, SP_HALF, UILayer::MID, "Back", NUM_SETTINGS_NG);
	Event_Component back_state_event_ng;
	back_state_event_ng.e.common.type = EventType::STATE_CHANGE;
	back_state_event_ng.e.state_change.new_states.push_front(ComponentTypeEnum::MAIN_MENU_STATE);
	back_state_event_ng.e.state_change.states_to_remove.push_front(ComponentTypeEnum::NEW_GAME_STATE);
	back_from_bg_button_c->triggered_events.push_back(back_state_event_ng);
	new_game_window_c->child_components_.push_back(back_from_bg_button_c);


	return 0;
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
	gCoordinator.RegisterComponent(ComponentTypeEnum::CAMERA_SINGLE);
	gCoordinator.RegisterComponent(ComponentTypeEnum::INPUT_CACHE);
	gCoordinator.RegisterComponent(ComponentTypeEnum::INPUT_TAGS);
	gCoordinator.RegisterComponent(ComponentTypeEnum::MOVEABLE);
	gCoordinator.RegisterComponent(ComponentTypeEnum::COLLISION);
	gCoordinator.RegisterComponent(ComponentTypeEnum::TILE);
	gCoordinator.RegisterComponent(ComponentTypeEnum::MAIN_MENU_STATE);
	gCoordinator.RegisterComponent(ComponentTypeEnum::MAIN_GAME_STATE);
	gCoordinator.RegisterComponent(ComponentTypeEnum::CORE_BG_GAME_STATE);
	gCoordinator.RegisterComponent(ComponentTypeEnum::MAIN_SETTINGS_STATE);
	gCoordinator.RegisterComponent(ComponentTypeEnum::NEW_GAME_STATE);
	gCoordinator.RegisterComponent(ComponentTypeEnum::EVENT);



	

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


	//Sprite Factory Creator system looks for Pre_Sprite_Factories and uses them create Sprite_Factories
	auto event_system = gCoordinator.RegisterSystem<Event_System>();
	{
		Signature signature;
		signature.set(gCoordinator.GetComponentType(ComponentTypeEnum::EVENT));
		gCoordinator.SetSystemSignatureActable<Event_System>(signature);
	}
	event_system->Init();



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
	Pre_Sprite_Factory* apocalypse_map_pre = new Pre_Sprite_Factory();
	std::string apoc_tile_manifest_path = g_media_folder + "/wasteland_tile/manifest.txt";
	apocalypse_map_pre->name_ = "APOCALYPSE_MAP";
	apocalypse_map_pre->tile_manifest_path_ = apoc_tile_manifest_path;
	gCoordinator.AddComponent(apoc_map_pre_entity, apocalypse_map_pre);
	gCoordinator.AddComponent(apoc_map_pre_entity, Create_Gamestate_Component_from_Enum(ComponentTypeEnum::CORE_BG_GAME_STATE));

	

	//todo save utility or game state
	/**
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
			game_map = SXNGN::ECS::A::JSON_Utils::json_to_tile_batch(val);
		}
	}
	

	auto game_map_pre_renders = std::get<0>(game_map);
	auto game_map_collisionables = std::get<1>(game_map);
	auto game_map_tiles = std::get<2>(game_map);

	for (int i = 0; i < game_map_pre_renders.size(); i++)
	{
		auto map_tile_entity = gCoordinator.CreateEntity();
		Pre_Renderable* pre_render = new Pre_Renderable(game_map_pre_renders.at(i));
		gCoordinator.AddComponent(map_tile_entity, pre_render);
		Collisionable* collisionable = new Collisionable(game_map_collisionables.at(i));
		gCoordinator.AddComponent(map_tile_entity, collisionable);
		Tile* tile = new Tile(game_map_tiles.at(i));
		gCoordinator.AddComponent(map_tile_entity, tile);

		gCoordinator.AddComponent(map_tile_entity, Create_Gamestate_Component_from_Enum(ComponentTypeEnum::MAIN_GAME_STATE));
		
	}
	**/
	std::forward_list<ComponentTypeEnum> active_game_states;
	active_game_states.clear();
	active_game_states.push_front(ComponentTypeEnum::MAIN_MENU_STATE);
	active_game_states.push_front(ComponentTypeEnum::CORE_BG_GAME_STATE);
	gCoordinator.GameStateChanged(active_game_states);
	init_menus();



	//std::cout << extracted << std::endl;

	//auto json_to_entity_test = SXNGN::ECS::A::JSON_Utils::json_to_component(extracted);
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

	SXNGN::ECS::A::CameraComponent::init_instance(camera_lens, camera_position, g_screen_bounds);


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
			SXNGN::ECS::A::User_Input_Cache* input_cache = new User_Input_Cache();
			input_cache->events_ = events_this_frame;
			gCoordinator.AddComponent(input_event, input_cache, true);
			gCoordinator.AddComponent(input_event, Create_Gamestate_Component_from_Enum(ComponentTypeEnum::CORE_BG_GAME_STATE), true);
			//update event handling system
			input_system->Update(dt);
		}

		event_system->Update(dt);

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
