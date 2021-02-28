#pragma once

#include <SDL.h>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <Constants.h>
#include "Camera.h"


//TODO this class should just handle graphics.
//New Terrain class should handle collision etc.
namespace SXNGN {

	class Texture;

	enum TileType
	{
		NORMAL,
		WALL,
		UNIT,
		TERRAIN,
		META_HEIGHT,
		META_WIDTH,
		UNKNOWN
	};


	//Class for objects represented by multiple tiles
	//Must be rectangular
	class MultiSprite
	{
	public:
		MultiSprite(std::vector<std::vector<std::shared_ptr<SXNGN::Tile>>> tile_matrix);

		std::vector<std::vector<std::shared_ptr<SXNGN::Tile>>> get_tiles();

		//Get the collision box
		std::shared_ptr<SDL_Rect> get_bounding_box();

		//Update the tiles. Recalculate the collision box.
		void update_tiles(std::vector<std::vector<std::shared_ptr<SXNGN::Tile>>>);

		//Render all the tiles
		void render(std::shared_ptr<SXNGN::Camera> camera, int x = -1, int y = -1);

	protected:
		std::shared_ptr<SDL_Rect> bounding_box_;
		std::vector<std::vector<std::shared_ptr<SXNGN::Tile>>> tile_matrix_;
	};

	//Tiles are the base graphical objects of the engine
	//Objects (characters, projectiles, etc) and terrain contain tiles that inform what the graphics look like
	class Tile
	{
	public:
		//Initializes position and type
		Tile(
			std::shared_ptr<SXNGN::Texture> tileTexture = nullptr,
			int x = 0, int y = 0,
			int tile_clip_x = 0, int tile_clip_y = 0,
			std::string tile_name = "Uninit Tile",
			int tile_width = SXNGN::BASE_TILE_WIDTH, int tile_height = SXNGN::BASE_TILE_HEIGHT,
			TileType tile_type = NORMAL);

		Tile(
			std::shared_ptr<SXNGN::Texture> tileTexture,
			std::shared_ptr<SDL_Rect> collision_box,
			std::string tile_name,
			std::shared_ptr<SDL_Rect> tile_clip_box,
			TileType tile_type);


		//Shows the tile
		void render(std::shared_ptr<SXNGN::Camera> camera);

		//Get the tile type
		TileType getType();


		std::string getTileName();

		//Get the collision box
		std::shared_ptr<SDL_Rect> getCollisionBox();

		 void setCollisionBox(std::shared_ptr<SDL_Rect>);

		//Get the tile clip box (shows where in the srpite sheet to get this tile's graphic)
		std::shared_ptr<SDL_Rect> getTileClipBox();


		std::shared_ptr<SXNGN::Texture> get_tile_texture();

		void set_tile_texture(std::shared_ptr<SXNGN::Texture> texture_in);



		
	private:
		//The attributes of the tile
		std::shared_ptr<SDL_Rect> collision_box_;
		//
		std::shared_ptr<SDL_Rect> tile_map_snip_;
		std::shared_ptr<SXNGN::Texture> tile_texture_;

		//The tile type
		TileType tile_type_;

		std::string tile_name_;
	};

	class TileHandler
	{
	public:
		//TileHandler(SDL_Renderer* renderer, std::string tileSheetPath, std::string tileMapPath, std::string tileNameListPath, unsigned int tileMapSize, unsigned int totalTiles, unsigned int tile_width, unsigned int tile_height);

		TileHandler(SDL_Renderer* renderer, std::string sourcePath);


		TileType TileTypeIntToEnum(int tile_type_int);
		bool initTileNames();
		bool initTilesNamesFromManifest();
		bool loadTileMap(std::vector<SXNGN::Tile>& tiles, std::string map_path);
		std::vector< std::vector<std::shared_ptr<SXNGN::Tile>>> loadTileMap2D(std::string map_path, bool& success);
		//void render(SDL_Rect& camera, Tile tile);
		bool setTileNameTileType(std::string tileName, SXNGN::TileType tileType);

		Tile generateTile(std::string);

		std::shared_ptr<SXNGN::Tile> generateTileRef(std::string tile_name);

	private:
		std::shared_ptr<SXNGN::Texture> tileTexture_;
		
		std::vector<SDL_Rect> tile_clips_;

		//each tileset directory should have the following files inside:

		//"map.map" the tile map - a bunch of tile types as integers that will make a map once parsed (water,water,land,land,land,wall,etc)
		//std::string tile_map_path_;
		//"tiles.png" the tile sheet - png with graphics for the tile types
		std::string tile_sheet_path_;
		//"tile_names.txt" the tile list - contains the tile_names to be mapped to a section of the tile map. Eg, put in "SAND" to the tile_type_map and get back an index to a chunk of the tilemap that looks like sand
		std::string tile_name_list_path_;

		//go from a map file to a string
		std::map<int, std::string> tile_name_int_to_string_map_;
		//where that string can go to a rect that is a tilesheet clip coordinate
		std::map< std::string, std::shared_ptr<SDL_Rect>> tile_name_string_to_rect_map;
		//go from a tilename "SAND, WATER" etc to a TileType that contains collision or other data "WALL" etc
		std::map<std::string, SXNGN::TileType> tile_name_string_to_tile_type_map_;
		
		//Instead of tile_names.txt, can use manifest.txt
		std::string tile_manifest_path_;

		const std::map<std::string, SXNGN::TileType> manifest_string_to_type_map_ = { {"TILE_HEIGHT",TileType::META_HEIGHT},{"TILE_WIDTH",TileType::META_WIDTH},{"TILE_UNIT",TileType::UNIT},{"TILE_TERRAIN",TileType::TERRAIN} };

		unsigned int total_tiles_, tile_width_, tile_height_, tile_map_size_;

		std::vector<SDL_Rect> tile_clips;

		std::vector< std::vector<std::shared_ptr<SXNGN::Tile>>> tile_1D_to_2D(std::vector<SXNGN::Tile> tiles, size_t width);

	};

}
