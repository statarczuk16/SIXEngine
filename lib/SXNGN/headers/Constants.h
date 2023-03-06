#pragma once
#include <string>
#include<sole.hpp>
namespace SXNGN {

	const std::string BAD_STRING_RETURN = "bad_string_result";

	//Tile constants
	const size_t BASE_TILE_WIDTH = 16;
	const size_t BASE_TILE_HEIGHT = 16;

	const size_t PIXELS_TO_METERS = 16;
	const size_t OVERWORLD_MULTIPLIER = 60; //for time and distance (1 second = 60 seconds in game, 1 m = 60 m in game)
	const double OVERWORLD_PIXELS_TO_METERS = double(PIXELS_TO_METERS) / OVERWORLD_MULTIPLIER; 
	const double OVERWORLD_METERS_TO_PIXEL = double(OVERWORLD_MULTIPLIER) / double(PIXELS_TO_METERS);
	const double OVERWORLD_PIXELS_TO_KM = OVERWORLD_PIXELS_TO_METERS * 1000.0;
	const double OVERWORLD_METERS_PER_GRID = 1000.0;
	const double OVERWORLD_PIXELS_PER_GRID = OVERWORLD_PIXELS_TO_METERS * OVERWORLD_METERS_PER_GRID;

	const size_t DEFAULT_WORLD_HEIGHT_CHUNKS = 4;
	const size_t DEFAULT_WORLD_WIDTH_CHUNKS = 4;

	const size_t DEFAULT_SCREEN_RES_WIDTH = 1280;
	const size_t DEFAULT_SCREEN_RES_HEIGHT = 720;


	const size_t TILE_WIDTH_SCALE = 3;
	const size_t TILE_HEIGHT_SCALE = 3;

	const size_t AT_DESTINATION_THRESH = 0;

	const size_t TILES_TO_CHUNK_EDGE = 16;

	const double G = 9.80; //meters per second squared gravity

	const sole::uuid BAD_UUID = sole::rebuild("00000000-0000-0000-0000-000000000000");
	const std::int32_t BAD_ENTITY = -1;

	const std::string DEFAULT_SPACE = "DEFAULT_SPACE";

	const std::string OVERWORLD_PACE_M_S = "OVERWORLD_PACE_M_S";
	const std::string OVERWORLD_GO = "OVERWORLD_GO";
	const std::string OVERWORLD_PACE_TOTAL_M_S = "OVERWORLD_PACE_TOTAL_M_S";

	const std::string OVERWORLD_PLAYER_UUID = "OVERWORLD_PLAYER_UUID";

	const std::string PARTY_STAMINA = "PARTY_STAMINA";
	const std::string PARTY_HEALTH = "PARTY_HEALTH";
	const std::string PARTY_FOOD = "PARTY_FOOD";

	const std::string PAUSE = "PAUSE";
	const std::string UNPAUSE = "UNPAUSE";

	const std::string CACHE = "_CACHE";

	

}