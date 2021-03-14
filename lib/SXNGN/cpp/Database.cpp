#include <Database.h>
#include <stdio.h>

Uint8 SXNGN::Database::scale_;
Uint8 SXNGN::Database::max_fps_;
std::shared_ptr<SXNGN::ECS::A::Coordinator> SXNGN::Database::ecs_coordinator_;

SXNGN::Database::Database()
{
	scale_ = 1;
	max_fps_ = 30;
	ecs_coordinator_ = nullptr;

}

Uint8 SXNGN::Database::get_scale()
{
	return scale_;
}

void SXNGN::Database::set_scale(Uint8 new_scale)
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


Uint8 SXNGN::Database::get_max_fps()
{
	return max_fps_;
 }

void SXNGN::Database::set_max_fps(Uint8 new_fps)
{
	max_fps_ = new_fps;
 }

Uint8  SXNGN::Database::get_screen_ticks_per_frame()
{
	if (max_fps_ == 0)
	{
		printf("Error: Database: max_fps_ is 0");
		return 60;
	}
	return 1000 / max_fps_;
 }


std::shared_ptr<SXNGN::ECS::A::Coordinator> SXNGN::Database::get_coordinator()
{
	return ecs_coordinator_;
}

 void SXNGN::Database::set_coordinator(std::shared_ptr<SXNGN::ECS::A::Coordinator> ecs_coordinator)
{
	ecs_coordinator_ = ecs_coordinator;
}


