#include <ECS/Components/Director.hpp>


namespace SXNGN::ECS {


	Director::Director()
	{
		component_type = ComponentTypeEnum::DIRECTOR;

		struct tm t2;
		t2 = { 0 };

		t2.tm_year = 208;
		t2.tm_hour = 12;
		t2.tm_min = 1;
		t2.tm_mon = 1;
		t2.tm_mday = 1;
		t2.tm_sec = 1;

		game_clock_ = mktime(&t2);
		event_gauge_s_ = 10.0;
		event_tick_s_ = 0.0;
		
	}


}