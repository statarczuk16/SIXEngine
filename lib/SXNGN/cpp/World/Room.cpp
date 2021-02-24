#pragma once

#include <World/Room.h>


SXNGN::Room::Room(std::vector< std::vector<std::shared_ptr<SXNGN::Tile>>> terrain_tiles)
{
	 
	 
	 update_tiles(terrain_tiles);
	 
}

std::vector<SXNGN::Tile> SXNGN::Room::get_tiles_1D()
{
	return cached_1D_tiles_;
}

void SXNGN::Room::update_tiles(std::vector< std::vector<std::shared_ptr<SXNGN::Tile>>> terrain_tiles)
{
	room_tiles_ = terrain_tiles;
	tile_width_ = room_tiles_.at(0).at(0)->getCollisionBox()->w;
	tile_height_ = room_tiles_.at(0).at(0)->getCollisionBox()->h;

	num_tiles_w_ = terrain_tiles.at(0).size();
	num_tiles_h_ = terrain_tiles.size();

	width_pixels_ = num_tiles_w_ * tile_width_;
	height_pixels_ = num_tiles_h_ * tile_width_;

	SDL_Rect room_bounds;
	room_bounds.x = 0;
	room_bounds.y = 0;
	room_bounds.w = int(width_pixels_);
	room_bounds.h = int(height_pixels_);

	cached_room_bounds_ = room_bounds;

	for (auto room_row : room_tiles_)
	{
		for (auto tile : room_row)
		{
			SXNGN::Tile temp = *tile;
			cached_1D_tiles_.push_back(temp);
		}
	}
}

SDL_Rect SXNGN::Room::get_room_bounds()
{
	return cached_room_bounds_;
}