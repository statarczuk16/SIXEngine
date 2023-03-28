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
	const double OVERWORLD_GRIDS_PER_PIXEL = 1 / OVERWORLD_PIXELS_PER_GRID;

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
	const std::string OVERWORLD_PAUSE = "OVERWORLD_PAUSE";
	const std::string OVERWORLD_LOST = "OVERWORLD_LOST";
	const std::string OVERWORLD_PLAYER_UUID = "OVERWORLD_PLAYER_UUID";

	//Party System Constants
	const std::string PARTY_STAMINA = "PARTY_STAMINA";
	const std::string PARTY_HEALTH = "PARTY_HEALTH";
	const std::string PARTY_FOOD = "PARTY_FOOD";

	const std::string PAUSE = "PAUSE";
	const std::string UNPAUSE = "UNPAUSE";
	const std::string UPDATE_PACE = "UPDATE_PACE";

	const std::string CACHE = "_CACHE";

	const std::string WORLD_MAP = "WORLD_MAP";
	const std::string OVERWORLD_PACE_PENALTY_M_S = "OVERWORLD_PACE_PENALTY_M_S";

	
	const double SECONDS_TO_HOURS = (1 / 60.0) * (1 / 60.0);
	const double HANDS_BASE_CALORIES_PER_KM = 100.0;
	const double LOST_BASE_KM = 1; //
	const double SICK_BASE_MINUTES_GM = 60.0 * 2; //game time minutes to be sick for when sick event happens
	const double WEATHER_BASE_MINUTES_GM = 60.0; //game time minutes for sandstorm/weather to last
	const double PARTY_RECHARGE_CALORIES_PER_MINUTE = 15.0;
	const double FOOD_UNITS_PER_CALORIES = 0.1;
	const double FOOD_CALORIES_PER_UNIT = 1 / FOOD_UNITS_PER_CALORIES;

	const double WORLD_ROAD_PENALTY_PACE_M_S = -0.5; //road bonus
	const double WORLD_DESERT_PENALTY_PACE_M_S = 0.1; //offroading penalty
	const double WORLD_NONE_PENALTY_PACE_M_S = 0.0; //offroading penalty
	const double PARTY_PACE_NOMINAL_M_S = 1.0; //no health or weight problems 
	const double PARTY_PACE_BOOTS_PENALTY_MILD = 0.5;

	const double PARTY_PACE_WEIGHT_PENALTY_MILD = 0.2;
	const double PARTY_PACE_WEIGHT_PENALTY_MEDIUM = 0.3;
	const double PARTY_PACE_WEIGHT_PENALTY_EXTREME = 1.4;

	const double PARTY_WEIGHT_CAPACITY_PER_HAND_KG = 100.0;

	//Moving while sick takes more stamina
	const double PARTY_SICK_ENCUMBER_MULTIPLY_MILD = 0.10;
	const double PARTY_SICK_ENCUMBER_MULTIPLY_MED = 0.20;
	const double PARTY_SICK_ENCUMBER_MULTIPLY_EXT = 0.40;
	const double PARTY_SICK_ENCUMBER_MULTIPLY_SPICY = 0.60;

	//Moving in bad weather drains health
	const double PARTY_WEATHER_HP_DRAIN_PER_HOUR_MILD = 20;
	const double PARTY_WEATHER_HP_DRAIN_PER_HOUR_MED = 40;
	const double PARTY_WEATHER_HP_DRAIN_PER_HOUR_EXT = 80;
	const double PARTY_WEATHER_HP_DRAIN_PER_SECOND_MILD = PARTY_WEATHER_HP_DRAIN_PER_HOUR_MILD * (1 / 60.0) * (1 / 60.0);
	const double PARTY_WEATHER_HP_DRAIN_PER_SECOND_MED = PARTY_WEATHER_HP_DRAIN_PER_HOUR_MED * (1 / 60.0) * (1 / 60.0);
	const double PARTY_WEATHER_HP_DRAIN_PER_SECOND_EXT = PARTY_WEATHER_HP_DRAIN_PER_HOUR_EXT * (1 / 60.0) * (1 / 60.0);
}