#pragma once

#include "..\headers\Database.h"
#include <stdio.h>

int SXNGN::Database::scale_;
int SXNGN::Database::max_fps_;

SXNGN::Database::Database()
{
	scale_ = 1;
	max_fps_ = 30;
}

int SXNGN::Database::get_scale()
{
	return scale_;
}

void SXNGN::Database::set_scale(int new_scale)
{
	scale_ = new_scale;
}

void SXNGN::Database::reduce_scale()
{
	if (scale_ > 1)
	{
		printf("Scale set to %d", scale_);
		scale_--;
	}
}

void SXNGN::Database::increase_scale()
{
	if (scale_ < 10)
	{
		printf("Scale set to %d", scale_);
		scale_++;
	}
}


int SXNGN::Database::get_max_fps()
{
	return max_fps_;
 }

void SXNGN::Database::set_max_fps(int new_fps)
{
	max_fps_ = new_fps;
 }

int  SXNGN::Database::get_screen_ticks_per_frame()
{
	if (max_fps_ == 0)
	{
		printf("Error: Database: max_fps_ is 0");
		return 60;
	}
	return 1000 / max_fps_;
 }


