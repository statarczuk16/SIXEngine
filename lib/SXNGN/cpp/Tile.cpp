#include <Tile.h>
#include <Collision.h>
#include <Texture.h>
#include <gameutils.h>


SXNGN::Tile::Tile(
	std::shared_ptr<SXNGN::Texture> tileTexture,
	int x, int y,
	int tile_clip_x, int tile_clip_y,
	std::string tile_name,
	int tile_width,	int tile_height,
	TileType tile_type)
{

	std::shared_ptr<SDL_Rect> collision_box_ = std::make_shared< SDL_Rect >();
	//Get the offsets
	collision_box_->x = x;
	collision_box_->y = y;

	//Set the collision box
	collision_box_->w = tile_width;
	collision_box_->h = tile_height;

	std::shared_ptr<SDL_Rect> tile_map_snip_ = std::make_shared< SDL_Rect >();
	tile_map_snip_->x = tile_clip_x;
	tile_map_snip_->y = tile_clip_y;

	tile_map_snip_->w = tile_width;
	tile_map_snip_->h = tile_height;


	//Get the tile type
	tile_type_ = tile_type;
	tile_name_ = tile_name;

	tile_texture_ = tileTexture;
}


SXNGN::Tile::Tile(
	std::shared_ptr<SXNGN::Texture> tileTexture,
	std::shared_ptr<SDL_Rect> collision_box,
	std::string tile_name,
	std::shared_ptr<SDL_Rect> tile_clip_box,
	TileType tile_type)
{
	//Get the offsets
	collision_box_ = collision_box;

	tile_map_snip_ = tile_clip_box;

	//Get the tile type
	tile_type_ = tile_type;
	tile_name_ = tile_name;

	tile_texture_ = tileTexture;
}




std::string SXNGN::Tile::getTileName()
{
	return tile_name_;
}

SXNGN::TileType SXNGN::Tile::getType()
{
	return tile_type_;
}

std::shared_ptr<SDL_Rect> SXNGN::Tile::getCollisionBox()
{
	return collision_box_;
}

void SXNGN::Tile::setCollisionBox(std::shared_ptr<SDL_Rect> box)
{
	collision_box_ = box;
}

std::shared_ptr<SDL_Rect> SXNGN::Tile::getTileClipBox()
{
	return tile_map_snip_;
}


void SXNGN::Tile::render(SDL_Rect& camera)
{
	//If the tile is on screen
	if (SXNGN::Collision::checkCollision(camera, *collision_box_))
	{
		//Show the tile
		tile_texture_.get()->render(collision_box_->x - camera.x, collision_box_->y - camera.y, tile_map_snip_.get());
	}
}
 


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

SXNGN::TileHandler::TileHandler(SDL_Renderer* renderer, std::string sourcePath)
{
	if (renderer == nullptr)
	{
		throw std::exception("Warn TileHandler:: pointer to renderer was null");
	}
	tileTexture_ = std::make_shared<SXNGN::Texture>(renderer);

	std::string manifest_path = sourcePath + "/manifest.txt";
	std::string tileSheetPath = sourcePath + "/tiles.png";

	//Load tile texture
	if (!tileTexture_->loadFromFile(tileSheetPath))
	{
		throw std::exception("Warn TileHandler:: Failed to load tile set texture");
	}

	if (!(Gameutils::file_exists(manifest_path)))
	{
		printf("Warn TileHandler::Init Failed to find tile manifest");
		throw std::exception("Warn TileHandler:: Failed to find tile manifest");
	}

	tile_manifest_path_ = manifest_path;

	bool success = initTilesNamesFromManifest();

	if (!success)
	{
		printf("Warn TileHandler:: failed to load tile manifest");
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


/**void SXNGN::TileHandler::render(SDL_Rect& camera, Tile tile)
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
}**/

bool SXNGN::TileHandler::setTileNameTileType(std::string tileName, SXNGN::TileType tileType)
{
	if (tile_name_string_to_rect_map.count(tileName) == 0)
	{
		std::cout << "Error: TileHandler: setTileNameTileType " << tileName << " has no entry for " << tileName << std::endl;
		return false;
	}
	else
	{
		tile_name_string_to_tile_type_map_[tileName] = tileType;
	}
	return true;
}

SXNGN::Tile SXNGN::TileHandler::generateTile(std::string tile_name)
{
	if (tile_name_string_to_rect_map.count(tile_name) > 0)
	{
		auto tile_snip_box = tile_name_string_to_rect_map[tile_name];
		std::shared_ptr<SDL_Rect> collision_box = std::make_shared< SDL_Rect>();
		collision_box->x = 0;
		collision_box->y = 0;
		collision_box->w = tile_snip_box->w;
		collision_box->h = tile_snip_box->h;
		Tile ret = Tile(tileTexture_, collision_box, tile_name, tile_snip_box, TileType::NORMAL);
		return ret;
	}
	else
	{
		throw std::exception("Warn generateTile: not tile of that name");
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

bool SXNGN::TileHandler::initTilesNamesFromManifest()
{
	bool tilesNamesLoaded = true;

	ifstream in(tile_manifest_path_);

	if (!in)
	{
		std::cout << "Fatal: Cannot open tile manifest: " << tile_manifest_path_ << std::endl;
		return false;
	}
	//Open the map
	std::string str;
	int tile_name_idx = 0;
	char delimiter = ',';
	std::string w = "";
	std::vector < std::vector< std::string> > manifest_vector;
	std::vector< std::string>  line_split_results;
	while (std::getline(in, str))
	{
		line_split_results.clear();
		w = "";
		for (auto rem : str)
		{
			if (rem == delimiter)
			{
				line_split_results.push_back(w);
				w = "";
			}
			else
			{
				w = w + rem;
			}
			
		}
		line_split_results.push_back(w);
		manifest_vector.push_back(line_split_results);
	}

	std::cout << "Parsing manifest for " << tile_manifest_path_ << std::endl;
	for (auto manifest_entry : manifest_vector)
	{
		
		if (manifest_entry.empty())
		{
			std::cout << "Empty line..." << std::endl;
			continue;
		}
		std::string manifest_entry_type_str = manifest_entry.at(0);
		std::cout << "Type: " << manifest_entry_type_str << std::endl;
		SXNGN::TileType manifest_tile_type;
		if (manifest_string_to_type_map_.count(manifest_entry_type_str) > 0)
		{
			manifest_tile_type = manifest_string_to_type_map_.at(manifest_entry_type_str);
		}
		else
		{
			std::cout << "Uknown manifest entry type: " << manifest_entry_type_str << std::endl;
			continue;
		}

		switch (manifest_tile_type)
		{
		case TileType::META_HEIGHT :
			{
				if (manifest_entry.size() == 2)
				{
					tile_height_ = std::stoi(manifest_entry.at(1));
				}
			}
		case TileType::META_WIDTH:
		{
			if (manifest_entry.size() == 2)
			{
				tile_width_ = std::stoi(manifest_entry.at(1));
			}
		}
		case TileType::TERRAIN:
		{
			if (manifest_entry.size() == 4)
			{
				std::shared_ptr<SDL_Rect> entry = std::make_shared<SDL_Rect>();
				int x_grid = std::stoi(manifest_entry.at(1));
				int y_grid = std::stoi(manifest_entry.at(2));
				std::string tile_name = manifest_entry.at(3);
				entry->x = x_grid * tile_width_;
				entry->y = y_grid * tile_height_;
				entry->w = tile_width_;
				entry->h = tile_height_;
				tile_name_string_to_rect_map[tile_name] = entry;
			}
		}
		case TileType::UNIT:
		{
			if (manifest_entry.size() == 4)
			{
				std::shared_ptr<SDL_Rect> entry = std::make_shared<SDL_Rect>();
				int x_grid = std::stoi(manifest_entry.at(1));
				int y_grid = std::stoi(manifest_entry.at(2));
				std::string tile_name = manifest_entry.at(3);
				entry->x = x_grid * tile_width_;
				entry->y = y_grid * tile_height_;
				entry->w = tile_width_;
				entry->h = tile_height_;
				tile_name_string_to_rect_map[tile_name] = entry;
			}
		}

		}
	}

	in.close();

	return true;
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
			SXNGN::TileType tile_type = SXNGN::TileType::NORMAL;
			std::shared_ptr<SDL_Rect> tile_clip_box;
			std::shared_ptr<SDL_Rect> tile_collision_box = std::make_shared < SDL_Rect>();

			//Read tile from map file
			map >> tile_name_int;

			if (map.eof())
			{
				continue;
			}

			//Make sure the int in the map corresponds to a tilename for this tilemap spritesheet
			//Eg, should be a map for every number to something like "DIRT" or "TOP_LEFT_CAR" 
			if (tile_name_int_to_string_map_.count(tile_name_int) != 1)
			{
				printf("Error loading map: Unexpected tile type\n");
				return false;
			}

			tile_name = tile_name_int_to_string_map_[tile_name_int];

			//See if there is a TileType mapped to the name
			//Eg "DESERT_CONCRETE_WALL" could be TileType::WALL
			if (tile_name_string_to_tile_type_map_.count(tile_name) > 0)
			{
				tile_type = tile_name_string_to_tile_type_map_[tile_name];
			}

			//See if there is a TileType mapped to the name
			//Eg "DESERT_CONCRETE_WALL" could be TileType::WALL
			if (tile_name_string_to_rect_map.count(tile_name) > 0)
			{
				tile_clip_box = tile_name_string_to_rect_map[tile_name];
			}


			//If the was a problem in reading the map
			if (map.fail())
			{
				//Stop loading map
				printf("Error loading map: Unexpected end of file!\n");
				tilesLoaded = false;
				break;
			}

			tile_collision_box->x = x;
			tile_collision_box->y = y;
			tile_collision_box->w = SXNGN::DEFAULT_TILE_WIDTH;
			tile_collision_box->h = SXNGN::DEFAULT_TILE_HEIGHT;

			//Create the tile and add it to output
			tiles.push_back(Tile(tileTexture_, tile_collision_box, tile_name, tile_clip_box, tile_type));


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



