#include <nlohmann/json.hpp>
#include <iostream>
#include <sole.hpp>
#include <SDL.h>
#include <fstream>
#include <Collision.h>
#include <ECS/Components/Director.hpp>
#include <ECS/Components/EventComponent.hpp>
#include "ECS/Systems/DirectorSystem.hpp"
using namespace SXNGN::ECS;


std::vector<std::function<void()>> tests;

int test_1()
{
	int radius = 5;
	SDL_FRect rect;
	rect.x = 10;
	rect.y = 0;
	rect.w = 5;
	rect.h = 5;

	SDL_FRect circle;
	circle.x = 0;
	circle.y = 0;
	circle.w = radius;
	circle.y = radius;

	for (int x = 0; x < 40; x++)
	{
		circle.x = x;
		bool collide = SXNGN::CollisionChecks::checkCollisionRectCircleBuffer(rect, circle, 0);

		std::cout << "Radius: " << radius << std::endl;
		for (int i = 0; i < 40; i++)
		{
			if (x == i)
			{
				std::cout << "O";
			}
			else if (x - radius == i)
			{
				std::cout << "(";
			}
			else if (x + radius == i)
			{
				std::cout << ")";
			}
			else
			{
				std::cout << "_";
			}
		}
		std::cout << std::endl;
		for (int i = 0; i < 40; i++)
		{
			if (i == rect.x)
			{
				std::cout << "[";
			}
			else if (i == rect.x + rect.w)
			{
				std::cout << "]";
			}
			else
			{
				std::cout << "_";
			}
		}
		std::cout << std::endl;
		std::cout << collide << std::endl;

		if (x + radius >= rect.x && x - radius <= rect.x + rect.w )
		{
			assert(collide == true);
		}
		else
		{
			assert(collide == false);
		}
	}

	


	return 0;

}

int test_2()
{
	SXNGN::ECS::Director test_director;

	auto director_system = SXNGN::ECS::Director_System();
	test_director.event_table_ = director_system.GenerateEventTable();
	

	std::map< int, int > result_map;
	for (int i = 0; i < 100; i++)
	{
		auto gen_event = test_director.event_table_.generate_event(&test_director.event_table_);
		std::cout << party_event_type_enum_to_string()[gen_event->value] << " " << gen_event->weight << std::endl;
		test_director.event_table_.print_event_table(test_director.event_table_);
		result_map[gen_event->weight] ++;
	}
	for (auto result_pair : result_map)
	{
		std::cout << "Weight: " << result_pair.first << " Frequency: " << result_pair.second << std::endl;
	}

	return 0;
}

int test_3()
{

	SXNGN::ECS::Director test_director;
	auto director_system = SXNGN::ECS::Director_System();
	test_director.event_table_ = director_system.GenerateEventTable();

	test_director.event_table_.print_event_table(test_director.event_table_);

	auto event_ptr = test_director.event_table_.find_event_by_type(PartyEventType::ROAD_BAD_BOOTS);

	assert(event_ptr != nullptr);

	

	return 0;
}

int test_4()
{
	std::vector<std::string > fails;
	for (int i = PartyEventType::NONE; i != PartyEventType::ANY_END; i++)
	{
		PartyEventType event_type = static_cast<PartyEventType>(i);
		//assert(party_event_type_enum_to_string().count(event_type) == 1);
		if (party_event_type_enum_to_string().count(event_type) != 1)
		{
			fails.push_back("party_event_type_enum_to_string missing " + std::to_string(event_type));
			continue;
		}
		std::string str = party_event_type_enum_to_string()[event_type];
		//assert(party_event_type_string_to_enum().count(str) == 1);
		if (party_event_type_string_to_enum().count(str) != 1)
		{
			fails.push_back("party_event_type_string_to_enum missing " + std::to_string(event_type));
			continue;
		}
		PartyEventType test = party_event_type_string_to_enum()[str];
		if (event_type != test)
		{
			fails.push_back("bad conversion for " + std::to_string(event_type));
			continue;
		}
		
	}
	for (auto fail : fails)
	{
		std::cout << fail << std::endl;
	}
	assert(fails.empty());

	for (Uint8 i = static_cast<Uint8>(ComponentTypeEnum::UNKNOWN); i != static_cast<Uint8>(ComponentTypeEnum::NUM_COMPONENT_TYPES); i++)
	{
		ComponentTypeEnum event_type = static_cast<ComponentTypeEnum>(i);
		assert(component_type_enum_to_string().count(event_type) == 1);
		std::string str = component_type_enum_to_string()[event_type];
		assert(component_type_string_to_enum().count(str) == 1);
		ComponentTypeEnum test = component_type_string_to_enum()[str];
		assert(event_type == test);
	}
	return 0;
}



int main(int argc, char* args[])
{
	tests.push_back(test_1);
	tests.push_back(test_2);
	tests.push_back(test_3);
	tests.push_back(test_4);

	for (auto test : tests)
	{
		test();
	}
	
	

	return 0;
}