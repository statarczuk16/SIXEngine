#include <Tile.h>
#include <Collision.h>
#include <Texture.h>
#include <gameutils.h>


SXNGN::Tile::Tile(int x, int y, std::string tile_name, int tile_width, int tile_height, TileType tile_type)
{
	//Get the offsets
	mBox.x = x;
	mBox.y = y;

	//Set the collision box
	mBox.w = tile_width;
	mBox.h = tile_height;

	//Get the tile type
	tile_type_ = tile_type;
	tile_name_ = tile_name;
}



std::string SXNGN::Tile::getTileName()
{
	return tile_name_;
}

SXNGN::TileType SXNGN::Tile::getType()
{
	return tile_type_;
}
std::string getTileName();

SDL_Rect SXNGN::Tile::getBox()
{
	return mBox;
}
/**

void SXNGN::Tile::render(SDL_Rect& camera, std::shared_ptr<SDL_Rect> tileType)
{
	//If the tile is on screen
	if (SXNGN::Collision::checkCollision(camera, mBox))
	{
		//Show the tile
		tile_texture_.render(mBox.x - camera.x, mBox.y - camera.y, tileType.get()]);
	}
}
 **/


SXNGN::TileHandler::TileHandler(SDL_Renderer* renderer, std::string tileSheetPath, std::string tileMapPath, std::string tileNameListPath, unsigned int tileMapSize, unsigned int totalTiles, unsigned int tile_width, unsigned int tile_height)
{
	if (renderer == nullptr)
	{
		throw std::exception("Warn TileHandler:: pointer to renderer was null");
	}
	tileTexture_ = std::make_shared<SXNGN::Texture>(renderer);

	//Load tile texture
	if (!tileTexture_->loadFromFile(tileSheetPath))
	{
		throw std::exception("Warn TileHandler:: Failed to load tile set texture");
	}

	if (!(Gameutils::file_exists(tileMapPath)))
	{
		printf("Warn TileHandler::Init Failed to find tile map");
		throw std::exception("Warn TileHandler:: Failed to find tile map");
	}

	if (!(Gameutils::file_exists(tileNameListPath)))
	{
		printf("Warn TileHandler::Init Failed to find tile map");
		throw std::exception("Warn TileHandler:: Failed to find tile list");
	}

	tile_name_list_path_ = tileNameListPath;
	tile_sheet_path_ = tileSheetPath;
	tile_map_path_ = tileMapPath;
	tile_map_size_ = tileMapSize;
	total_tiles_ = totalTiles;
	tile_width_ = tile_width;
	tile_height_ = tile_height;

	bool tileNamesLoaded = initTileNames();

	if (!tileNamesLoaded)
	{
		printf("Warn TileHandler:: failed to load tile names");
		throw std::exception("Warn TileHandler:: Failed to load tile names");
	}
}


SXNGN::TileType SXNGN::TileHandler::TileTypeIntToEnum(int tile_type_int)
{
	switch (tile_type_int)
	{
	case 0: return TileType::NORMAL;
	case 1: return TileType::WALL;
	default: return TileType::NORMAL;
	}
}


void SXNGN::TileHandler::render(SDL_Rect& camera, Tile tile)
{
	if (tile_name_string_to_rect_map.count(tile.getTileName()) != 1)
	{
		std::cout << "SXNGN::TileHandler::render: Error Tile had unknown type of " << tile.getTileName() << std::endl;
		return;
	}
	auto tileType = tile_name_string_to_rect_map[tile.getTileName()];
	//If the tile is on screen
	if (Collision::checkCollision(camera, tile.getBox()))
	{
		//Show the tile
		tileTexture_.get()->render(tile.getBox().x - camera.x, tile.getBox().y - camera.y, tileType.get());
	}
}

bool SXNGN::TileHandler::setTileNameTileType(std::string tileName, SXNGN::TileType tileType)
{
	if (tile_name_string_to_rect_map.count(tileName) == 0)
	{
		std::cout << "Error: TileHandler: setTileNameTileType " << tileName << " has no entry for " << tileName << std::endl;
		return false;
	}
	else
	{
		tile_name_int_to_tile_type_map_[tileName] = tileType;
	}
}


bool SXNGN::TileHandler::initTileNames()
{

	bool tilesNamesLoaded = true;

	ifstream in(tile_name_list_path_);

	if (!in) 
	{
		std::cout << "Cannot open tile type list";
		return false;
	}
	//Open the map
	std::string str;
	int tile_name_idx = 0;
	while (std::getline(in, str)) 
	{
		// output the line
		std::cout << "tile type: " << str << " mapped to " << tile_name_idx << std::endl;
		tile_name_int_to_string_map_[tile_name_idx] = str;
		tile_name_idx++;
		// now we loop back and get the next line in 'str'
	}

	in.close();

	//Clip the sprite sheet
	if (tilesNamesLoaded)
	{

		/** convert the map of ints to strings into a map that can take a tile_name eg "SAND" and return coordinates to that tile in the tile_map
		* Eg
		* ["SAND"] -> [Rect.x = 0 Rect.y = 0 Rect.w = tile_width Rect.y = tile_height]
		*
		*
		**/


		// Create a map iterator and point to beginning of map
		std::map<int, std::string>::iterator it = tile_name_int_to_string_map_.begin();
		// Iterate over the map using c++11 range based for loop
		int x_tilemap = 0;
		int y_tilemap = 0;
		for (std::pair<int, std::string> element : tile_name_int_to_string_map_) 
		{
			std::shared_ptr<SDL_Rect> entry = std::make_shared<SDL_Rect>();
			entry->x = x_tilemap;
			entry->y = y_tilemap;
			entry->w = tile_width_;
			entry->h = tile_height_;
			tile_name_string_to_rect_map[element.second] = entry;

			x_tilemap += tile_width_;
			if (x_tilemap > 240) //FIXME dont hardcode - limits to 4 tiles across
			{
				x_tilemap = 0;
				y_tilemap += tile_height_;
			}
		}
	}
	//Close the file
	//If the map was loaded fine
	return tilesNamesLoaded;
}



bool SXNGN::TileHandler::loadTileMap(std::vector<SXNGN::Tile>& tiles, SDL_Rect level_bounds)
{
	//Success flag
	bool tilesLoaded = true;

	//The tile offsets
	int x = 0, y = 0;

	std::ifstream map(tile_map_path_);

	//If the map couldn't be loaded
	if (map.fail())
	{
		printf("Unable to load map file!\n");
		tilesLoaded = false;
	}
	else
	{
		int x_bound_max = level_bounds.x + level_bounds.w;
		int x_bound_min = level_bounds.x;
		int y_bound_max = level_bounds.y + level_bounds.h;
		int y_bound_min = level_bounds.y;
		//Initialize the tiles
		for (int i = 0; !map.eof(); i++)
		{
			//Determines what kind of tile will be made
			int tile_name_int = -1;
			std::string tile_name = BAD_STRING_RETURN;

			//Read tile from map file
			map >> tile_name_int;

			if (map.eof())
			{
				continue;
			}

			if (tile_name_int_to_string_map_.count(tile_name_int) != 1)
			{
				printf("Error loading map: Unexpected tile type\n");
				return false;
			}

			tile_name = tile_name_int_to_string_map_[tile_name_int];

			//If the was a problem in reading the map
			if (map.fail())
			{
				//Stop loading map
				printf("Error loading map: Unexpected end of file!\n");
				tilesLoaded = false;
				break;
			}


			tiles.push_back(Tile(x, y, tile_name));


			//Move to next tile spot
			x += tile_width_;

			//If we've gone too far
			if (x >= x_bound_max)
			{
				//Move back
				x = 0;

				//Move to the next row
				y += tile_height_;
			}
			if (y > y_bound_max)
			{
				printf("Error loading map: Too many tiles for level bounds\n");
				return false;
			}
		}
	}
	return tilesLoaded;
}



