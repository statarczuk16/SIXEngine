//#pragma warning(N:4596)


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
#include <ECS/Systems/CollisionSystem.hpp>
#include <ECS/Systems/TaskSchedulerSystem.hpp>
#include <string.h>
#include <ECS/Systems/ParallaxSystem.hpp>



SXNGN::ECS::A::Coordinator gCoordinator;


static bool quit = false;

std::string g_media_folder = SXNGN::BAD_STRING_RETURN;//todo put in database?

std::string g_save_folder = SXNGN::BAD_STRING_RETURN;//todo put in database?

SDL_Renderer* gRenderer = NULL;

SXNGN::Database g_database; //static can be accessed anywhere, but must intialize



using Gameutils = SXNGN::Gameutils;
using nlohmann::json;
using entity_builder = SXNGN::ECS::A::Entity_Builder_Utils;


using Movement_System = SXNGN::ECS::A::Movement_System;

using namespace SXNGN::ECS::A;

std::shared_ptr<UIContainerComponent> debug_fps_actual;
 
std::shared_ptr<UIContainerComponent> input_system_ms;
std::shared_ptr<UIContainerComponent> task_scheduler_ms;
std::shared_ptr<UIContainerComponent> movement_system_ms;
std::shared_ptr<UIContainerComponent> collision_system_ms;
std::shared_ptr<UIContainerComponent> event_system_ms;
std::shared_ptr<UIContainerComponent> render_system_ms;
std::shared_ptr<UIContainerComponent> ecs_stats_num_entities;
 
std::shared_ptr<UIContainerComponent> input_system_label;
std::shared_ptr<UIContainerComponent> task_scheduler_label;
std::shared_ptr<UIContainerComponent> movement_system_label;
std::shared_ptr<UIContainerComponent> collision_system_label;
std::shared_ptr<UIContainerComponent> event_system_label;
std::shared_ptr<UIContainerComponent> render_system_label;
std::shared_ptr<UIContainerComponent> ecs_stats_label;




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

		
		gCoordinator.InitStateManager();
		auto settings = gCoordinator.get_state_manager()->getGameSettings();
		
		gRenderer = kiss_init("HOPLON", &kiss_objects, settings->resolution.w, settings->resolution.h, kiss_resource_folder.c_str(), tile_set.c_str());
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


int init_menus()
{

	const int BUTTON_WIDTH = 150;
	const int BUTTON_HEIGHT = 50;

	const int STAT_LABEL_HEIGHT = 25;
	auto coordinator = SXNGN::Database::get_coordinator();
	const int WINDOW_HEIGHT = 620;
	const int WINDOW_WIDTH = 320;
	auto ui = UICollectionSingleton::get_instance();
	auto settings = coordinator->get_state_manager()->getGameSettings();
	auto resolution = settings->resolution;
	//************************* Main Menu
	auto mmw_c = UserInputUtils::create_window_raw(nullptr, 220, resolution.h - WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT, UILayer::BOTTOM);
	ui->add_ui_element(ComponentTypeEnum::MAIN_MENU_STATE, mmw_c);

	auto start_button_c = UserInputUtils::create_button(mmw_c->window_, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "New Game", 1, -1, BUTTON_WIDTH, BUTTON_HEIGHT);
	Event_Component new_game_event;
	new_game_event.e.common.type = EventType::STATE_CHANGE;
	new_game_event.e.state_change.new_states.push_front(ComponentTypeEnum::NEW_GAME_STATE);
	new_game_event.e.state_change.states_to_remove.push_front(ComponentTypeEnum::MAIN_MENU_STATE);
	start_button_c->triggered_events.push_back(new_game_event);
	mmw_c->child_components_.push_back(start_button_c);

	auto load_button_c = UserInputUtils::create_button(mmw_c->window_, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "Load Game", 2, -1, BUTTON_WIDTH, BUTTON_HEIGHT);
	Event_Component load_game_event;
	load_game_event.e.common.type = EventType::LOAD;
	load_game_event.e.load.filePath = "No File Path";
	load_button_c->triggered_events.push_back(load_game_event);
	mmw_c->child_components_.push_back(load_button_c);

	

	auto settings_button_c = UserInputUtils::create_button(mmw_c->window_, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "Settings", 4, -1, BUTTON_WIDTH, BUTTON_HEIGHT);
	Event_Component settings_state_event;
	settings_state_event.e.common.type = EventType::STATE_CHANGE;
	settings_state_event.e.state_change.new_states.push_front(ComponentTypeEnum::MAIN_SETTINGS_STATE);
	settings_state_event.e.state_change.states_to_remove.push_front(ComponentTypeEnum::MAIN_MENU_STATE);
	settings_button_c->triggered_events.push_back(settings_state_event);
	settings_button_c->triggered_events.push_back(settings_state_event);
	mmw_c->child_components_.push_back(settings_button_c);

	auto exit_button_c = UserInputUtils::create_button(mmw_c->window_, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::MID, "Exit", 5, -1, BUTTON_WIDTH, BUTTON_HEIGHT);
	Event_Component exit_button_event;
	exit_button_event.e.common.type = EventType::EXIT;
	exit_button_c->triggered_events.push_back(exit_button_event);
	mmw_c->child_components_.push_back(exit_button_c);


	//************************* Settings Menu

	auto msw_container = UserInputUtils::create_window_raw(nullptr, 220, resolution.h - WINDOW_HEIGHT, WINDOW_WIDTH*2, WINDOW_HEIGHT, UILayer::BOTTOM);
	ui->add_ui_element(ComponentTypeEnum::MAIN_SETTINGS_STATE, msw_container);

	const int NUM_SETTINGS = 9;
	for (int i = 0; i < NUM_SETTINGS; i++)
	{
		auto h_align = HA_COLUMN;
		auto v_align = VA_ROW;
		int row = i / 2;
		int column = i % 2;
		auto parent_scale = SP_HALF;
		auto setting_button_c_row = UserInputUtils::create_button(msw_container->window_, h_align, v_align, parent_scale, UILayer::MID, (char*)("Setting " + std::to_string(i)).c_str(), row, column, BUTTON_WIDTH, BUTTON_HEIGHT);
		msw_container->child_components_.push_back(setting_button_c_row);
	}

	auto back_button_c = UserInputUtils::create_button(msw_container->window_, HA_CENTER, VA_ROW, SP_HALF, UILayer::MID, "Back", NUM_SETTINGS, -1, BUTTON_WIDTH, BUTTON_HEIGHT);
	
	Event_Component back_state_event;
	back_state_event.e.common.type = EventType::STATE_CHANGE;
	back_state_event.e.state_change.new_states.push_front(ComponentTypeEnum::MAIN_MENU_STATE);
	back_state_event.e.state_change.states_to_remove.push_front(ComponentTypeEnum::MAIN_SETTINGS_STATE);
	back_button_c->triggered_events.push_back(back_state_event);
	back_button_c->triggered_events.push_back(back_state_event);
	msw_container->child_components_.push_back(back_button_c);

	//************************* New Game Menu

	const int NUM_SETTINGS_NG = 9;
	auto new_game_window_c = UserInputUtils::create_window_raw(nullptr, 220, resolution.h - WINDOW_HEIGHT, WINDOW_WIDTH * 2, WINDOW_HEIGHT, UILayer::BOTTOM);
	ui->add_ui_element(ComponentTypeEnum::NEW_GAME_STATE, new_game_window_c);

	//New Game Label
	auto ngl_c = UserInputUtils::create_label(new_game_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_THIRD, UILayer::MID, "New Game", 0, -1, BUTTON_WIDTH, BUTTON_HEIGHT);
	new_game_window_c->child_components_.push_back(ngl_c);

	auto lwel_c = UserInputUtils::create_label(new_game_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_THIRD, UILayer::MID, "Level Width (Chunks)", 1, 1, BUTTON_WIDTH, BUTTON_HEIGHT);
	new_game_window_c->child_components_.push_back(lwel_c);

	auto lwe_c = UserInputUtils::create_num_entry(new_game_window_c->window_, HA_COLUMN, VA_ROW, SP_THIRD, UILayer::MID, "level_width_entry", 2, 24, TE_INT, 4, 2, 1);
	new_game_window_c->child_components_.push_back(lwe_c);

	auto lhel_c = UserInputUtils::create_label(new_game_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_THIRD, UILayer::MID, "Level Height (Chunks)", 3, 1, BUTTON_WIDTH, BUTTON_HEIGHT);
	new_game_window_c->child_components_.push_back(lhel_c);

	auto lhe_c = UserInputUtils::create_num_entry(new_game_window_c->window_, HA_COLUMN, VA_ROW, SP_THIRD, UILayer::MID, "level_height_entry", 2, 24, TE_INT, 4, 4, 1);
	new_game_window_c->child_components_.push_back(lhe_c);

	
	//Callback functions for level width/height 
	std::function<void(std::shared_ptr<UIContainerComponent> uicc)> ng_update_width = [coordinator](std::shared_ptr<UIContainerComponent> uicc)
	{
		coordinator->get_state_manager()->setLevelWidthTiles(Uint32(uicc->entry_->num_val));
	};

	std::function<void()> ng_update_width_b = std::bind(ng_update_width, lwe_c);
	lwe_c->callback_functions_.push_back(ng_update_width_b);

	//Callback functions for level width/height 
	std::function<void(std::shared_ptr<UIContainerComponent> uicc)> ng_update_height = [coordinator](std::shared_ptr<UIContainerComponent> uicc)
	{
		coordinator->get_state_manager()->setLevelHeightTiles(Uint32(uicc->entry_->num_val));
	};

	std::function<void()> ng_update_height_b = std::bind(ng_update_height, lhe_c);
	lhe_c->callback_functions_.push_back(ng_update_height_b);
		
	auto new_game_start_b = UserInputUtils::create_button(new_game_window_c->window_, HA_CENTER, VA_ROW, SP_HALF, UILayer::MID, "Start", NUM_SETTINGS_NG-1, -1, BUTTON_WIDTH, BUTTON_HEIGHT);
	//Callback functions for level width/height 
	/**std::function<void(std::shared_ptr<UIContainerComponent> uicc)> ng_start_game = [coordinator](std::shared_ptr<UIContainerComponent> uicc)
	{
		
	};**/

	std::function<void()> ng_start = Map_Utils::StartNewGame;
	new_game_start_b->callback_functions_.push_back(ng_start);
	new_game_window_c->child_components_.push_back(new_game_start_b);
		

	auto back_from_bg_button_c = UserInputUtils::create_button(new_game_window_c->window_, HA_CENTER, VA_ROW, SP_HALF, UILayer::MID, "Back", NUM_SETTINGS_NG, -1, BUTTON_WIDTH, BUTTON_HEIGHT);
	Event_Component back_state_event_ng;
	back_state_event_ng.e.common.type = EventType::STATE_CHANGE;
	back_state_event_ng.e.state_change.new_states.push_front(ComponentTypeEnum::MAIN_MENU_STATE);
	back_state_event_ng.e.state_change.states_to_remove.push_front(ComponentTypeEnum::NEW_GAME_STATE);
	back_from_bg_button_c->triggered_events.push_back(back_state_event_ng);
	new_game_window_c->child_components_.push_back(back_from_bg_button_c);

	//************************* In Game UI

	const int MAIN_GAME_STATE_MENU_HEIGHT = 80;
	// Window
	auto ig_ui_window_top_c = UserInputUtils::create_window_raw(nullptr, 0, 0, resolution.w, MAIN_GAME_STATE_MENU_HEIGHT, UILayer::BOTTOM);
	ui->add_ui_element(ComponentTypeEnum::MAIN_GAME_STATE, ig_ui_window_top_c);
	//offset the game world display below this menu strip at the top
	std::shared_ptr<SDL_Rect> overworld_viewport = coordinator->get_state_manager()->getStateViewPort(ComponentTypeEnum::MAIN_GAME_STATE);
	overworld_viewport->y = MAIN_GAME_STATE_MENU_HEIGHT;
	coordinator->get_state_manager()->setStateViewPort(ComponentTypeEnum::MAIN_GAME_STATE, overworld_viewport);

	//Open Menu Button
	auto ig_go_to_menu_button = UserInputUtils::create_button(ig_ui_window_top_c->window_, HA_COLUMN, VA_ROW, SP_NONE, UILayer::MID, "Menu",0,3, BUTTON_WIDTH, BUTTON_HEIGHT);
	ig_ui_window_top_c->child_components_.push_back(ig_go_to_menu_button);

	

	std::function<void(double new_pace)> set_pace = [](double new_pace)
	{
		SXNGN::Database::property_map_.operator[](SXNGN::OVERWORLD_PACE) = new_pace;
	};
	std::function<void()> set_pace_stop = std::bind(set_pace, 0.0);
	std::function<void()> set_pace_slow = std::bind(set_pace, -1.0);
	std::function<void()> set_pace_medium = std::bind(set_pace, -2.0);
	std::function<void()> set_pace_fast = std::bind(set_pace, -3.0);


	auto ig_pace_0 = UserInputUtils::create_button(ig_ui_window_top_c->window_, HA_COLUMN, VA_ROW, SP_NONE, UILayer::MID, "Stop", 0, 4, BUTTON_WIDTH, BUTTON_HEIGHT);
	ig_ui_window_top_c->child_components_.push_back(ig_pace_0);
	ig_pace_0->callback_functions_.push_back(set_pace_stop);

	auto ig_pace_1 = UserInputUtils::create_button(ig_ui_window_top_c->window_, HA_COLUMN, VA_ROW, SP_NONE, UILayer::MID, "Slow", 0, 5, BUTTON_WIDTH, BUTTON_HEIGHT);
	ig_ui_window_top_c->child_components_.push_back(ig_pace_1);
	ig_pace_1->callback_functions_.push_back(set_pace_slow);

	auto ig_pace_2 = UserInputUtils::create_button(ig_ui_window_top_c->window_, HA_COLUMN, VA_ROW, SP_NONE, UILayer::MID, "Medium", 0, 6, BUTTON_WIDTH, BUTTON_HEIGHT);
	ig_ui_window_top_c->child_components_.push_back(ig_pace_2);
	ig_pace_2->callback_functions_.push_back(set_pace_medium);

	auto ig_pace_3 = UserInputUtils::create_button(ig_ui_window_top_c->window_, HA_COLUMN, VA_ROW, SP_NONE, UILayer::MID, "Fast", 0, 7, BUTTON_WIDTH, BUTTON_HEIGHT);
	ig_ui_window_top_c->child_components_.push_back(ig_pace_3);
	ig_pace_3->callback_functions_.push_back(set_pace_fast);

	auto ig_debug_toggle_1 = UserInputUtils::create_select_button(ig_ui_window_top_c->window_, HA_COLUMN, VA_ROW, SP_NONE, UILayer::MID, "Debug_Spawn_Block", 0, 0, BUTTON_WIDTH, BUTTON_HEIGHT);

	ig_ui_window_top_c->child_components_.push_back(ig_debug_toggle_1);

	//************************* Pop Up Game Menu
	auto ig_ui_window_pop_up_c = UserInputUtils::create_window_raw(nullptr, resolution.w/2 - 240/2, 85, 240, resolution.h - 240, UILayer::MID);
	ig_ui_window_pop_up_c->window_->visible = false;
	ui->add_ui_element(ComponentTypeEnum::MAIN_GAME_STATE, ig_ui_window_pop_up_c);
	//Callback function to toggle menu visiblility 
	std::function<void(std::shared_ptr<UIContainerComponent> uicc)> toggle_menu = [coordinator](std::shared_ptr<UIContainerComponent> uicc)
	{
		uicc->window_->visible = !uicc->window_->visible;
	};
	std::function<void()> mg_toggle_menu_visible = std::bind(toggle_menu, ig_ui_window_pop_up_c);

	//References In Game Menu
	ig_go_to_menu_button->callback_functions_.push_back(mg_toggle_menu_visible);

	//Exit to Desktop
	auto pop_up_menu_exit_button_c = UserInputUtils::create_button(ig_ui_window_pop_up_c->window_, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::TOP, "Exit To Desktop", 3, -1, BUTTON_WIDTH, BUTTON_HEIGHT);
	pop_up_menu_exit_button_c->triggered_events.push_back(exit_button_event);
	ig_ui_window_pop_up_c->child_components_.push_back(pop_up_menu_exit_button_c);

	//Exit to Main Menu
	auto pop_up_menu_exit_to_main_button_c = UserInputUtils::create_button(ig_ui_window_pop_up_c->window_, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::TOP, "Exit To Menu", 2, -1, BUTTON_WIDTH, BUTTON_HEIGHT);
	pop_up_menu_exit_to_main_button_c->callback_functions_.push_back(mg_toggle_menu_visible);
	ig_ui_window_pop_up_c->child_components_.push_back(pop_up_menu_exit_to_main_button_c);

	Event_Component main_menu_exit_event;
	main_menu_exit_event.e.common.type = EventType::STATE_CHANGE;
	main_menu_exit_event.e.state_change.hard_remove = true;
	main_menu_exit_event.e.state_change.new_states.push_front(ComponentTypeEnum::MAIN_MENU_STATE);
	main_menu_exit_event.e.state_change.states_to_remove.push_front(ComponentTypeEnum::MAIN_GAME_STATE);
	pop_up_menu_exit_to_main_button_c->triggered_events.push_back(main_menu_exit_event);


	//Close Menu
	auto pop_up_menu_close_menu_button_c = UserInputUtils::create_button(ig_ui_window_pop_up_c->window_, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::TOP, "Resume", 0, -1, BUTTON_WIDTH, BUTTON_HEIGHT);
	pop_up_menu_close_menu_button_c->callback_functions_.push_back(mg_toggle_menu_visible);
	ig_ui_window_pop_up_c->child_components_.push_back(pop_up_menu_close_menu_button_c);


	//Save Button
	auto save_button_c = UserInputUtils::create_button(ig_ui_window_pop_up_c->window_, HA_CENTER, VA_ROW, SP_FILL_WITH_BUFFER, UILayer::TOP, "Save", 1, -1, BUTTON_WIDTH, BUTTON_HEIGHT);
	Event_Component save_game_event;
	save_game_event.e.common.type = EventType::SAVE;
	save_game_event.e.save.filePath = "No File Path";
	save_button_c->triggered_events.push_back(save_game_event);
	ig_ui_window_pop_up_c->child_components_.push_back(save_button_c);






	//************************* Debug Overlay
	auto debug_window_c = UserInputUtils::create_window_raw(nullptr, resolution.w-80, MAIN_GAME_STATE_MENU_HEIGHT, 80, resolution.h - MAIN_GAME_STATE_MENU_HEIGHT, UILayer::MID);
	std::shared_ptr<UIContainerComponent> debug_fps_l = UserInputUtils::create_label(debug_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_THIRD, UILayer::TOP, "FPS", 0, -1, BUTTON_WIDTH, STAT_LABEL_HEIGHT);
	debug_window_c->child_components_.push_back(debug_fps_l);

	debug_fps_actual = UserInputUtils::create_label(debug_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_THIRD, UILayer::TOP, "N/A", 1, -1, BUTTON_WIDTH, STAT_LABEL_HEIGHT);
	debug_window_c->child_components_.push_back(debug_fps_actual);



	input_system_label = UserInputUtils::create_label(debug_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_THIRD, UILayer::TOP, "INPUT", 2, -1, BUTTON_WIDTH, STAT_LABEL_HEIGHT);
	debug_window_c->child_components_.push_back(input_system_label);
	task_scheduler_label = UserInputUtils::create_label(debug_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_THIRD, UILayer::TOP, "TASK", 4, -1, BUTTON_WIDTH, STAT_LABEL_HEIGHT);
	debug_window_c->child_components_.push_back(task_scheduler_label);
	movement_system_label = UserInputUtils::create_label(debug_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_THIRD, UILayer::TOP, "MOVEMENT", 6, -1, BUTTON_WIDTH, STAT_LABEL_HEIGHT);
	debug_window_c->child_components_.push_back(movement_system_label);
	collision_system_label = UserInputUtils::create_label(debug_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_THIRD, UILayer::TOP, "COLLISION", 8, -1, BUTTON_WIDTH, STAT_LABEL_HEIGHT);
	debug_window_c->child_components_.push_back(collision_system_label);
	event_system_label = UserInputUtils::create_label(debug_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_THIRD, UILayer::TOP, "EVENT", 10, -1, BUTTON_WIDTH, STAT_LABEL_HEIGHT);
	debug_window_c->child_components_.push_back(event_system_label);
	render_system_label = UserInputUtils::create_label(debug_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_THIRD, UILayer::TOP, "RENDER", 12, -1, BUTTON_WIDTH, STAT_LABEL_HEIGHT);
	debug_window_c->child_components_.push_back(render_system_label);
	ecs_stats_label = UserInputUtils::create_label(debug_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_THIRD, UILayer::TOP, "ENTITIES", 14, -1, BUTTON_WIDTH, STAT_LABEL_HEIGHT);
	debug_window_c->child_components_.push_back(ecs_stats_label);

	 input_system_ms = UserInputUtils::create_label(debug_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_THIRD, UILayer::TOP, "N/A 1", 3, -1, BUTTON_WIDTH, STAT_LABEL_HEIGHT);
	 debug_window_c->child_components_.push_back(input_system_ms);
	 task_scheduler_ms= UserInputUtils::create_label(debug_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_THIRD, UILayer::TOP, "N/A 2", 5, -1, BUTTON_WIDTH, STAT_LABEL_HEIGHT);
	 debug_window_c->child_components_.push_back(task_scheduler_ms);
	 movement_system_ms = UserInputUtils::create_label(debug_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_THIRD, UILayer::TOP, "N/A ", 7, -1, BUTTON_WIDTH, STAT_LABEL_HEIGHT);
	 debug_window_c->child_components_.push_back(movement_system_ms);
	 collision_system_ms = UserInputUtils::create_label(debug_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_THIRD, UILayer::TOP, "N/A", 9, -1, BUTTON_WIDTH, STAT_LABEL_HEIGHT);
	 debug_window_c->child_components_.push_back(collision_system_ms);
	 event_system_ms = UserInputUtils::create_label(debug_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_THIRD, UILayer::TOP, "N/A", 11, -1, BUTTON_WIDTH, STAT_LABEL_HEIGHT);
	 debug_window_c->child_components_.push_back(event_system_ms);
	 render_system_ms = UserInputUtils::create_label(debug_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_THIRD, UILayer::TOP, "N/A", 13, -1, BUTTON_WIDTH, STAT_LABEL_HEIGHT);
	 debug_window_c->child_components_.push_back(render_system_ms);
	 ecs_stats_num_entities = UserInputUtils::create_label(debug_window_c->window_, HA_CENTER, HA_CENTER, VA_ROW, SP_THIRD, UILayer::TOP, "N/A", 15, -1, BUTTON_WIDTH, STAT_LABEL_HEIGHT);
	 debug_window_c->child_components_.push_back(ecs_stats_num_entities);

	 ui->add_ui_element(ComponentTypeEnum::CORE_BG_GAME_STATE, debug_window_c);



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

	auto settings = gCoordinator.get_state_manager()->getGameSettings();
	SDL_FRect camera_lens;
	camera_lens.h = settings->resolution.h;
	camera_lens.w = settings->resolution.w;
	camera_lens.x = 0;
	camera_lens.y = 0;

	SDL_FRect camera_position;
	camera_position.x = 0;
	camera_position.y = 0;
	camera_position.w = 0;
	camera_position.h = 0;

	//singleton, used by world creation utility to lock camera onto main character
	auto main_camera_comp = SXNGN::ECS::A::CameraComponent::init_instance(camera_lens, camera_position, settings->resolution);

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
	gCoordinator.RegisterComponent(ComponentTypeEnum::PARALLAX);
	gCoordinator.RegisterComponent(ComponentTypeEnum::MAIN_MENU_STATE);
	gCoordinator.RegisterComponent(ComponentTypeEnum::MAIN_GAME_STATE);
	gCoordinator.RegisterComponent(ComponentTypeEnum::CORE_BG_GAME_STATE);
	gCoordinator.RegisterComponent(ComponentTypeEnum::MAIN_SETTINGS_STATE);
	gCoordinator.RegisterComponent(ComponentTypeEnum::TACTICAL_STATE);
	gCoordinator.RegisterComponent(ComponentTypeEnum::NEW_GAME_STATE);
	gCoordinator.RegisterComponent(ComponentTypeEnum::TASK);
	gCoordinator.RegisterComponent(ComponentTypeEnum::TASK_WORKER);
	gCoordinator.RegisterComponent(ComponentTypeEnum::EVENT);
	gCoordinator.RegisterComponent(ComponentTypeEnum::SELECTABLE);
	gCoordinator.RegisterComponent(ComponentTypeEnum::LOCATION);



	

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

	//Collision system handles collision between entities but also clicks and draggable boxes etc with things in game world
	auto collision_system = gCoordinator.RegisterSystem<Collision_System>();
	{
		//actable - has collision and can move
		Signature signature;
		signature.set(gCoordinator.GetComponentType(ComponentTypeEnum::COLLISION));
		//signature.set(gCoordinator.GetComponentType(ComponentTypeEnum::MOVEABLE));
		gCoordinator.SetSystemSignatureActable<Collision_System>(signature);
		//check against - anything that has collision
		Signature signature_of_interest;
		signature_of_interest.set(gCoordinator.GetComponentType(ComponentTypeEnum::COLLISION));
		gCoordinator.SetSystemSignatureOfInterest<Collision_System>(signature_of_interest);
	}
	collision_system->Init();


	auto input_system = gCoordinator.RegisterSystem<User_Input_System>();
	{
		//User_Input_System ACTS on any entity that has INPUT_CACHE (it USES input_cache)
		Signature signature_actable;
		signature_actable.set(gCoordinator.GetComponentType(ComponentTypeEnum::INPUT_CACHE));
		gCoordinator.SetSystemSignatureActable<User_Input_System>(signature_actable);
		//User_Input_System is INTERESTED in any entity that has INPUT_TAGS (It uses input_cache to apply input to entities according to their INPUT_TAGS)
		Signature signature_of_interest;
		signature_of_interest.set(gCoordinator.GetComponentType(ComponentTypeEnum::INPUT_CACHE));
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

	//Parallax system moves the background
	auto parallax_system = gCoordinator.RegisterSystem<Parallax_System>();
	{
		Signature signature;
		signature.set(gCoordinator.GetComponentType(ComponentTypeEnum::PARALLAX));
		gCoordinator.SetSystemSignatureActable<Parallax_System>(signature);
	}
	parallax_system->Init();

	//Task Scheduler System assigns workers to tasks
	auto task_scheduler_system = gCoordinator.RegisterSystem<Task_Scheduler_System>();
	{
		Signature signature;
		//ACTS on Tasks
		signature.set(gCoordinator.GetComponentType(ComponentTypeEnum::TASK));
		gCoordinator.SetSystemSignatureActable<Task_Scheduler_System>(signature);
		//INTERESTED in any entity that has TASK_WORKER
		Signature signature_of_interest;
		signature_of_interest.set(gCoordinator.GetComponentType(ComponentTypeEnum::TASK_WORKER));
		gCoordinator.SetSystemSignatureOfInterest<Task_Scheduler_System>(signature_of_interest);
	}
	task_scheduler_system->Init();



	SDL_Event e;

	//std::vector<Entity> entities(MAX_ENTITIES - 1);

	g_media_folder = "uninit";
	g_media_folder = Gameutils::find_folder_in_project_string("media");
	if (g_media_folder == SXNGN::BAD_STRING_RETURN)
	{
		std::cout << "Fatal: " << " Could not find media folder" << std::endl;
		return 0;
	}


	//Build the apocalypse map sprite factory for the tactical map
	auto apoc_map_pre_entity = gCoordinator.CreateEntity();
	Pre_Sprite_Factory* apocalypse_map_pre = new Pre_Sprite_Factory();
	std::string apoc_tile_manifest_path = g_media_folder + "/wasteland_tile/manifest.txt";
	apocalypse_map_pre->name_ = "APOCALYPSE_MAP";
	apocalypse_map_pre->tile_manifest_path_ = apoc_tile_manifest_path;
	gCoordinator.AddComponent(apoc_map_pre_entity, apocalypse_map_pre);
	gCoordinator.AddComponent(apoc_map_pre_entity, Create_Gamestate_Component_from_Enum(ComponentTypeEnum::CORE_BG_GAME_STATE));

	//Build the apocalypse map sprite factory for the overworld map
	auto dune_map_pre_entity = gCoordinator.CreateEntity();
	Pre_Sprite_Factory* dune_map_pre = new Pre_Sprite_Factory();
	std::string overworld_tile_manifest_path = g_media_folder + "/overworld/manifest.txt";
	dune_map_pre->name_ = "OVERWORLD_MAP";
	dune_map_pre->tile_manifest_path_ = overworld_tile_manifest_path;
	gCoordinator.AddComponent(dune_map_pre_entity, dune_map_pre);
	gCoordinator.AddComponent(dune_map_pre_entity, Create_Gamestate_Component_from_Enum(ComponentTypeEnum::CORE_BG_GAME_STATE));

	
	std::forward_list<ComponentTypeEnum> active_game_states;
	active_game_states.clear();
	active_game_states.push_front(ComponentTypeEnum::MAIN_MENU_STATE);
	active_game_states.push_front(ComponentTypeEnum::CORE_BG_GAME_STATE);
	gCoordinator.GameStateChanged(active_game_states);
	init_menus();

	SXNGN::Timer move_timer;//time passed between movement system calls

	SXNGN::Timer frame_timer;//use to calculate frame rate
	SXNGN::Timer frame_cap_timer;//use to enforce frame cap
	SXNGN::Timer system_timer;//use to time each system
	frame_timer.start();
	system_timer.start();

	float fps_avg = 0.0;
	float dt = 0.0;
	float game_dt;

	int frame_count = 0;
	std::vector<SDL_Event> events_this_frame;
	gCoordinator.updateCollisionMap();
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
		system_timer.start();
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
		strncpy(input_system_ms->label_->text, std::to_string(system_timer.getTicks() / 1000.f).c_str(), KISS_MAX_LENGTH);
		

		/////////////////////////////////Physics/Movement
		//Phys start
		dt = move_timer.getTicks() / 1000.f;//
		game_dt = dt;
		//todo - game_dt = 0.0 if paused - some systems don't operate when paused but some do
		

		system_timer.start();
		movement_system->Update(dt);
		parallax_system->Update(dt);
		task_scheduler_system->Update(dt);
		move_timer.start(); //restart delta t for next frame
		strncpy(movement_system_ms->label_->text, std::to_string(system_timer.getTicks() / 1000.f).c_str(),KISS_MAX_LENGTH);

		system_timer.start();
		collision_system->Update(dt);
		strncpy(collision_system_ms->label_->text, std::to_string(system_timer.getTicks() / 1000.f).c_str(),KISS_MAX_LENGTH);
		
		//Phys End
		/////////////////////////////////Event System - respond to physics, input, etc
		system_timer.start();
		event_system->Update(dt);
		strncpy(event_system_ms->label_->text, std::to_string(system_timer.getTicks() / 1000.f).c_str(),KISS_MAX_LENGTH);

		system_timer.start();
		
		strncpy(task_scheduler_ms->label_->text, std::to_string(system_timer.getTicks() / 1000.f).c_str(),KISS_MAX_LENGTH);
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
		system_timer.start();
		renderer_system->Update(dt);
		strncpy(render_system_ms->label_->text, std::to_string(system_timer.getTicks() / 1000.f).c_str(),KISS_MAX_LENGTH);

		strncpy(ecs_stats_num_entities->label_->text, std::to_string(gCoordinator.GetLivingEntityCount()).c_str(),KISS_MAX_LENGTH);

		auto stopTime = std::chrono::high_resolution_clock::now();
		
		//Render End
		SDL_RenderPresent(gRenderer);
		
		/////////////////////////////////Frame End
		fps_avg = frame_count / (frame_timer.getTicks() / 1000.f);
		if (fps_avg > 2000000)
		{
			fps_avg = 0;
		}
		strncpy(debug_fps_actual->label_->text, std::to_string(fps_avg).c_str(),KISS_MAX_LENGTH);
		if (frame_cap_timer.getTicks() < SXNGN::Database::get_screen_ticks_per_frame())
		{
			SDL_Delay(SXNGN::Database::get_screen_ticks_per_frame() - frame_cap_timer.getTicks());
		}
		
		++frame_count;

	}


	return 0;
}
