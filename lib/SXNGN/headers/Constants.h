#pragma once
#include <string>
namespace SXNGN {

	const std::string BAD_STRING_RETURN = "bad_string_result";

	//Tile constants
	const size_t BASE_TILE_WIDTH = 16;
	const size_t BASE_TILE_HEIGHT = 16;

	const size_t PIXELS_TO_METERS = 16;

	const size_t DEFAULT_WORLD_HEIGHT_CHUNKS = 4;
	const size_t DEFAULT_WORLD_WIDTH_CHUNKS = 4;

	const size_t DEFAULT_SCREEN_RES_WIDTH = 1600;
	const size_t DEFAULT_SCREEN_RES_HEIGHT = 1024;


	const size_t TILE_WIDTH_SCALE = 3;
	const size_t TILE_HEIGHT_SCALE = 3;

	const size_t AT_DESTINATION_THRESH = BASE_TILE_WIDTH;

	const size_t TILES_TO_CHUNK_EDGE = 16;

	const double G = 9.80; //meters per second squared gravity

}