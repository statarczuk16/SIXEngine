#include <Sprite/Tile.h>
#include <Texture.h>
#include <gameutils.h>
#include <fstream>




SXNGN::MultiSprite::MultiSprite(std::vector<std::vector<std::shared_ptr<SXNGN::Tile>>> tile_matrix)
{
	tile_matrix_ = tile_matrix;

	update_tiles(tile_matrix);
	SDL_Rect bounding_box;
	bounding_box.x = 0;
	bounding_box.y = 0;
	//tile_matrix must be rectangular
	bounding_box.w = tile_matrix.at(0).size() * SXNGN::BASE_TILE_WIDTH;
	bounding_box.h = tile_matrix.size() * SXNGN::BASE_TILE_HEIGHT;
}

void SXNGN::MultiSprite::update_tiles(std::vector<std::vector<std::shared_ptr<SXNGN::Tile>>> tile_matrix)
{
	tile_matrix_ = tile_matrix;
	SDL_Rect bounding_box;
	bounding_box.x = 0;
	bounding_box.y = 0;
	//tile_matrix must be rectangular
	bounding_box.w = tile_matrix.at(0).size() * SXNGN::BASE_TILE_WIDTH;
	bounding_box.h = tile_matrix.size() * SXNGN::BASE_TILE_HEIGHT;
	bounding_box_ = std::make_shared<SDL_Rect>(bounding_box);
}

void SXNGN::MultiSprite::render(std::shared_ptr<SXNGN::Camera> camera, int x, int y)
{
	//Location information should note be stored in the sprite, but just in case...
	if (x == -1)
	{
		x = bounding_box_->x;
	}
	if (y == -1)
	{
		y = bounding_box_->y;
	}

	for (auto sprite_vector : tile_matrix_)
	{
		for (auto sprite : sprite_vector)
		{
			sprite->getCollisionBox()->x = x;
			sprite->getCollisionBox()->y = y;
			sprite->render(camera);
			x += sprite->getCollisionBox()->w;
		}
		y += sprite_vector.at(0)->getCollisionBox()->h;
	}

}

SXNGN::Tile::Tile(
	std::shared_ptr<SXNGN::Texture> tileTexture,
	int x, int y,
	int tile_clip_x, int tile_clip_y,
	std::string tile_name,
	int tile_width, int tile_height,
	TileType tile_type)
{

	collision_box_ = std::make_shared< SDL_Rect >();
	//Get the offsets
	collision_box_->x = x;
	collision_box_->y = y;

	//Set the collision box
	collision_box_->w = tile_width;
	collision_box_->h = tile_height;

	tile_map_snip_ = std::make_shared< SDL_Rect >();
	tile_map_snip_->x = tile_clip_x;
	tile_map_snip_->y = tile_clip_y;

	tile_map_snip_->w = tile_width;
	tile_map_snip_->h = tile_height;


	//Get the tile type
	tile_type_ = tile_type;
	renderable_name_ = tile_name;

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
	renderable_name_ = tile_name;

	tile_texture_ = tileTexture;
}




std::string SXNGN::Tile::getTileName()
{
	return renderable_name_;
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

std::shared_ptr<SXNGN::Texture> SXNGN::Tile::get_tile_texture()
{
	return tile_texture_;
}

void SXNGN::Tile::set_tile_texture(std::shared_ptr<SXNGN::Texture> texture_in)
{
	tile_texture_ = texture_in;
}


void SXNGN::Tile::render(std::shared_ptr<SXNGN::Camera> camera)
{
	//If the tile is on screen
	if (camera->object_in_view(*collision_box_))
	{
		//Raw coordinates of the camera without regarding scaling
		//Cannot compared scaled coordinates to raw coordinate in next step
		SDL_Rect camera_lens = camera->get_lens_rect_actual();
		//get the position of this object with respect to the camera lens
		int texture_pos_wrt_cam_x = collision_box_->x - camera_lens.x;
		int texture_pos_wrt_cam_y = collision_box_->y - camera_lens.y;

		tile_texture_.get()->render(texture_pos_wrt_cam_x, texture_pos_wrt_cam_y, tile_map_snip_.get());
	}
}

SXNGN::TileHandler::TileHandler(SDL_Renderer* renderer, std::string sourcePath)
{
	if (renderer == nullptr)
	{
		throw std::exception();
	}
	tileTexture_ = std::make_shared<SXNGN::Texture>(renderer);

	std::string manifest_path = sourcePath + "/manifest.txt";
	std::string tileSheetPath = sourcePath + "/tiles.png";

	//Load tile texture
	if (!tileTexture_->loadFromFile(tileSheetPath))
	{
		throw std::exception();
	}

	if (!(Gameutils::file_exists(manifest_path)))
	{
		printf("Warn TileHandler::Init Failed to find tile manifest");
		throw std::exception();
	}

	tile_manifest_path_ = manifest_path;

	bool success = initTilesNamesFromManifest();

	if (!success)
	{
		printf("Warn TileHandler:: failed to load tile manifest");
		throw std::exception();
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
	    printf("Warn generateTile: not tile of name: %s",tile_name.c_str());
		throw std::exception();
	}
}

std::shared_ptr<SXNGN::Tile> SXNGN::TileHandler::generateTileRef(std::string tile_name)
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
		std::shared_ptr<SXNGN::Tile> ret_ptr = std::make_shared<SXNGN::Tile>(ret);
		return ret_ptr;
	}
	else
	{
		return nullptr;
	}
}


bool SXNGN::TileHandler::initTileNames()
{

	bool tilesNamesLoaded = true;

	std::ifstream in(tile_name_list_path_);

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

	std::ifstream in(tile_manifest_path_);

	if (!in)
	{
		printf("Fatal: Cannot open tile manifest: %s\n", tile_manifest_path_.c_str());;
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
				if (w != "")
				{
					line_split_results.push_back(w);
				}

				w = "";
			}
			else
			{
				w = w + rem;
			}

		}
		if (w != "")
		{
			line_split_results.push_back(w);
		}
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
		case TileType::META_HEIGHT:
		{
			if (manifest_entry.size() == 2)
			{
				tile_height_ = std::stoi(manifest_entry.at(1));
			}
			else
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error loading TileManifest: META_HEIGHT");
			}
			break;
		}
		case TileType::META_WIDTH:
		{
			if (manifest_entry.size() == 2)
			{
				tile_width_ = std::stoi(manifest_entry.at(1));
			}
			else
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error loading TileManifest: META_WIDTH");
			}
			break;
		}
		case TileType::TERRAIN:
		{
			if (manifest_entry.size() == 5)
			{
				std::shared_ptr<SDL_Rect> entry = std::make_shared<SDL_Rect>();
				int x_grid = std::stoi(manifest_entry.at(1));
				int y_grid = std::stoi(manifest_entry.at(2));
				std::string tile_name = manifest_entry.at(3);
				int int_to_name = std::stoi(manifest_entry.at(4));
				entry->x = x_grid * tile_width_;
				entry->y = y_grid * tile_height_;
				entry->w = tile_width_;
				entry->h = tile_height_;
				tile_name_string_to_rect_map[tile_name] = entry;
				tile_name_int_to_string_map_[int_to_name] = tile_name;
			}
			else
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error loading TileManifest: TERRAIN");
			}
			break;
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
			else
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error loading TileManifest: UNIT");
			}
			break;
		}

		}
	}

	in.close();

	return true;
}


std::vector< std::vector<std::shared_ptr<SXNGN::Tile>>> SXNGN::TileHandler::loadTileMap2D(std::string map_path, bool& success)
{
	std::vector<SXNGN::Tile> map_1D;
	std::vector< std::vector<std::shared_ptr<SXNGN::Tile>>> map_2D;
	success = true;
	success = loadTileMap(map_1D, map_path);
	if (!success)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "loadTileMap2D - 1D Map Failed for %s", map_path.c_str());
		return map_2D;
	}

	size_t width = -1;
	for (size_t i =0; i< map_1D.size()-1; i++)
	{
		if (map_1D.at(i).getCollisionBox()->y != map_1D.at(i + 1).getCollisionBox()->y)
		{
			SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "loadTileMap2D - width of %zd for %s",width, map_path.c_str());
			width = i;
			break;
		}
	}

	if (width == -1)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "loadTileMap2D - 1D Map Failed for %s (no width found)", map_path.c_str());
		success = false;
		return map_2D;
	}

	map_2D = (tile_1D_to_2D(map_1D, width));
	success = true;
	return map_2D;






}

bool SXNGN::TileHandler::loadTileMap(std::vector<SXNGN::Tile>& tiles, std::string map_path)
{
	//Success flag
	bool tilesLoaded = true;

	//The tile offsets
	int x = 0, y = 0;


	std::ifstream map_stream(map_path);

	if (!map_stream)
	{
		printf("Fatal: Cannot open tile map: %s", map_path.c_str());
		return false;
	}
	//Open the map
	std::string str;
	int tile_name_idx = 0;
	char delimiter = ',';
	std::string w = "";
	std::vector < std::vector< int> > map_vector;
	std::vector< int>  line_split_results;
	while (std::getline(map_stream, str))
	{
		line_split_results.clear();
		w = "";
		for (auto rem : str)
		{
			if (rem == delimiter)
			{
				if (w != "")
				{
					line_split_results.push_back(std::stoi(w));
				}

				w = "";
			}
			else
			{
				w = w + rem;
			}

		}
		if (w != "")
		{
			line_split_results.push_back(std::stoi(w));
		}
		map_vector.push_back(line_split_results);
	}

	printf("Map is %zd tiles wide\n", map_vector.at(0).size());
	size_t x_bound_max_tiles = map_vector.at(0).size();
	size_t x_bound_min_tiles = 0;
	size_t y_bound_min_tiles = 0;
	//Initialize the tiles

	for (auto map_line : map_vector)
	{
		for (auto tile_name_int : map_line)
		{
			//Determines what kind of tile will be made
			std::string tile_name = BAD_STRING_RETURN;
			SXNGN::TileType tile_type = SXNGN::TileType::NORMAL;
			std::shared_ptr<SDL_Rect> tile_clip_box;
			std::shared_ptr<SDL_Rect> tile_collision_box = std::make_shared < SDL_Rect>();

			//Make sure the int in the map corresponds to a tilename for this tilemap spritesheet
			//Eg, should be a map for every number to something like "DIRT" or "TOP_LEFT_CAR"
			if (tile_name_int_to_string_map_.count(tile_name_int) != 1)
			{
				printf("Error loading map: Unexpected tile type\n");
				map_stream.close();
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

			tile_collision_box->x = x;
			tile_collision_box->y = y;
			tile_collision_box->w = SXNGN::BASE_TILE_WIDTH;
			tile_collision_box->h = SXNGN::BASE_TILE_HEIGHT;

			//Create the tile and add it to output
			tiles.push_back(Tile(tileTexture_, tile_collision_box, tile_name, tile_clip_box, tile_type));


			//Move to next tile spot
			x += tile_width_;

			//If we've gone too far
			if (x > ((x_bound_max_tiles-1) * tile_width_))
			{
				//Move back
				x = 0;

				//Move to the next row
				y += tile_height_;
			}
		}
	}
	map_stream.close();
	return tilesLoaded;
}

std::vector< std::vector<std::shared_ptr<SXNGN::Tile>>> SXNGN::TileHandler::tile_1D_to_2D(std::vector<SXNGN::Tile> tiles, size_t width)
{
	//Turn this [ABCDEFG]
	//int [ABC/DEF/G]
	std::vector< std::vector<std::shared_ptr<SXNGN::Tile>>> output_map;
	size_t row_idx = 0;
	std::vector<std::shared_ptr<SXNGN::Tile>> row;
	for(int i = 0; i< tiles.size(); i++)
	{
		std::shared_ptr<SXNGN::Tile> tile_ptr = std::make_shared<SXNGN::Tile>(tiles.at(i));
		row.push_back(tile_ptr);
		row_idx++;
		if (row_idx > width)
		{
			output_map.push_back(row);
			row.clear();
			row_idx = 0;
		}
	}
	return output_map;
}



