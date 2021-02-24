#pragma once

#include "../../headers/Object/Object.h"
#include "../../headers/Object/Entity.h"
#include "../../headers/Tile.h"


namespace SXNGN {
	//The dot that will move around on the screen
	class Room
	{
	public:
		//Initializes the variables
		Room(std::vector< std::vector<std::shared_ptr<SXNGN::Tile>>> terrain_tiles);

		std::vector<SXNGN::Tile> get_tiles_1D();

		SDL_Rect get_room_bounds();

		




	protected:
		//The tiles that make up the terrain of this room
		std::vector< std::vector<std::shared_ptr<SXNGN::Tile>>> room_tiles_;
		std::vector<SXNGN::Tile> cached_1D_tiles_;
		SDL_Rect cached_room_bounds_;

		void update_tiles(std::vector< std::vector<std::shared_ptr<SXNGN::Tile>>> terrain_tiles);

		size_t tile_width_ = 0;
		size_t tile_height_ = 0;

		size_t num_tiles_w_ = 0;
		size_t num_tiles_h_ = 0;

		size_t width_pixels_ = 0;
		size_t height_pixels_ = 0;
	};
}