#pragma once

#include <SDL.h>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <Constants.h>

//The tile
namespace SXNGN {

	class Texture;

	enum TileType
	{
		NORMAL,
		WALL,
		UNKNOWN
	};


	class Tile
	{
	public:
		//Initializes position and type
		Tile(int x, int y, std::string tileName, int tile_width = SXNGN::DEFAULT_TILE_WIDTH, int tile_height = SXNGN::DEFAULT_TILE_HEIGHT, TileType tileType = NORMAL);

		//Shows the tile
		//void render(SDL_Rect& camera);

		//Get the tile type
		TileType getType();

		std::string getTileName();

		//Get the collision box
		SDL_Rect getBox();

		

		

	private:
		//The attributes of the tile
		SDL_Rect mBox;

		//The tile type
		TileType tile_type_;

		std::string tile_name_;
	};

	class TileHandler
	{
	public:
		TileHandler(SDL_Renderer* renderer, std::string tileSheetPath, std::string tileMapPath, std::string tileNameListPath, unsigned int tileMapSize, unsigned int totalTiles, unsigned int tile_width, unsigned int tile_height);
		TileType TileTypeIntToEnum(int tile_type_int);
		bool initTileNames();
		bool loadTileMap(std::vector<SXNGN::Tile>& tiles, SDL_Rect level_bounds);
		void render(SDL_Rect& camera, Tile tile);
		bool setTileNameTileType(std::string tileName, SXNGN::TileType tileType);

	private:
		std::shared_ptr<SXNGN::Texture> tileTexture_;
		
		std::vector<SDL_Rect> tile_clips_;

		//each tileset directory should have the following files inside:

		//"map.map" the tile map - a bunch of tile types as integers that will make a map once parsed (water,water,land,land,land,wall,etc)
		std::string tile_map_path_;
		//"tiles.png" the tile sheet - png with graphics for the tile types
		std::string tile_sheet_path_;
		//"tile_names.txt" the tile list - contains the tile_names to be mapped to a section of the tile map. Eg, put in "SAND" to the tile_type_map and get back an index to a chunk of the tilemap that looks like sand
		std::string tile_name_list_path_;

		//go from a map file to a string
		std::map<int, std::string> tile_name_int_to_string_map_;
		//where that string can go to a rect that is a tilesheet clip coordinate
		std::map< std::string, std::shared_ptr<SDL_Rect>> tile_name_string_to_rect_map;
		//go from a tilename "SAND, WATER" etc to a TileType that contains collision or other data "WALL" etc
		std::map<int, SXNGN::TileType> tile_name_int_to_tile_type_map_;
		

		unsigned int total_tiles_, tile_width_, tile_height_, tile_map_size_;

		std::vector<SDL_Rect> tile_clips;

	};

}
