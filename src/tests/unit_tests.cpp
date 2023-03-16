#include <nlohmann/json.hpp>
#include <iostream>
#include <sole.hpp>
#include <SDL.h>
#include <fstream>
#include <Collision.h>
#include <ECS/Components/Director.hpp>
#include <ECS/Components/EventComponent.hpp>
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


	DropEntry<PartyEventType> event_table;

	DropEntry<PartyEventType> none_event;
	none_event.weight = 51;
	none_event.reoccurance_penalty = 10;
	none_event.accumulation = 50;
	none_event.value = PartyEventType::NONE;

	SXNGN::ECS::DropEntry<PartyEventType> good_events;
	good_events.weight = 0;
	good_events.accumulation = 0;
	good_events.value = PartyEventType::GOOD;
	good_events.children.push_back(none_event);
	DropEntry<PartyEventType> bad_events;
	bad_events.weight = 50;
	bad_events.value = PartyEventType::BAD;
	//bad_events.children.push_back(none_event);
	DropEntry<PartyEventType> neutral_events;
	neutral_events.weight = 0;
	neutral_events.accumulation = 0;
	neutral_events.value = PartyEventType::NEUTRAL;
	neutral_events.children.push_back(none_event);

	for (int i = PartyEventType::BAD + 1; i != PartyEventType::GOOD; i++)
	{
		PartyEventType event_type = static_cast<PartyEventType>(i);
		DropEntry<PartyEventType> event_entry;
		event_entry.weight = i * 3;
		event_entry.value = event_type;
		bad_events.children.push_back(event_entry);
	}
	for (int i = PartyEventType::GOOD + 1; i != PartyEventType::NEUTRAL; i++)
	{
		PartyEventType event_type = static_cast<PartyEventType>(i);
		DropEntry<PartyEventType> event_entry;
		event_entry.weight = 10;
		event_entry.value = event_type;
		good_events.children.push_back(event_entry);
	}
	for (int i = PartyEventType::NEUTRAL + 1; i != PartyEventType::NONE; i++)
	{
		PartyEventType event_type = static_cast<PartyEventType>(i);
		DropEntry<PartyEventType> event_entry;
		event_entry.weight = 10;
		event_entry.value = event_type;
		neutral_events.children.push_back(event_entry);
	}


	event_table.children.push_back(bad_events);
	event_table.children.push_back(good_events);
	event_table.children.push_back(neutral_events);
	event_table.children.push_back(none_event);
	event_table.weight = 1;

	event_table.print_event_table(event_table);


	test_director.event_table_.children.push_back(event_table);
	
	test_director.event_table_.print_event_table(test_director.event_table_);

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

	std::cout << "Test finding event by type in DropTable" << std::endl;
	SXNGN::ECS::Director test_director;


	DropEntry<PartyEventType> event_table;

	DropEntry<PartyEventType> none_event;
	none_event.weight = 51;
	none_event.reoccurance_penalty = 10;
	none_event.accumulation = 50;
	none_event.value = PartyEventType::NONE;

	SXNGN::ECS::DropEntry<PartyEventType> good_events;
	good_events.weight = 0;
	good_events.accumulation = 0;
	good_events.value = PartyEventType::GOOD;
	good_events.children.push_back(none_event);
	DropEntry<PartyEventType> bad_events;
	bad_events.weight = 50;
	bad_events.value = PartyEventType::BAD;
	//bad_events.children.push_back(none_event);
	DropEntry<PartyEventType> neutral_events;
	neutral_events.weight = 0;
	neutral_events.accumulation = 0;
	neutral_events.value = PartyEventType::NEUTRAL;
	neutral_events.children.push_back(none_event);

	for (int i = PartyEventType::BAD + 1; i != PartyEventType::GOOD; i++)
	{
		PartyEventType event_type = static_cast<PartyEventType>(i);
		DropEntry<PartyEventType> event_entry;
		event_entry.weight = i * 3;
		event_entry.value = event_type;
		bad_events.children.push_back(event_entry);
	}
	for (int i = PartyEventType::GOOD + 1; i != PartyEventType::NEUTRAL; i++)
	{
		PartyEventType event_type = static_cast<PartyEventType>(i);
		DropEntry<PartyEventType> event_entry;
		event_entry.weight = 10;
		event_entry.value = event_type;
		good_events.children.push_back(event_entry);
	}
	for (int i = PartyEventType::NEUTRAL + 1; i != PartyEventType::NONE; i++)
	{
		PartyEventType event_type = static_cast<PartyEventType>(i);
		DropEntry<PartyEventType> event_entry;
		event_entry.weight = 10;
		event_entry.value = event_type;
		neutral_events.children.push_back(event_entry);
	}


	event_table.children.push_back(bad_events);
	event_table.children.push_back(good_events);
	event_table.children.push_back(neutral_events);
	event_table.children.push_back(none_event);
	event_table.weight = 1;

	event_table.print_event_table(event_table);


	test_director.event_table_.children.push_back(event_table);

	test_director.event_table_.print_event_table(test_director.event_table_);

	auto event_ptr = test_director.event_table_.find_event_by_type(PartyEventType::BAD_BOOTS);

	assert(event_ptr != nullptr);

	

	return 0;
}



int main(int argc, char* args[])
{
	tests.push_back(test_1);
	tests.push_back(test_2);
	tests.push_back(test_3);

	for (auto test : tests)
	{
		test();
	}
	
	

	return 0;
}