#include <nlohmann/json.hpp>
#include <iostream>
#include <sole.hpp>
#include <SDL.h>
#include <fstream>

using nlohmann::json;
int main(int argc, char* argv[])
{
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
	std::cout << "///////////////////// Test 1" << std::endl;

	json j = json{
		{"basic_string_key","basic_string_val"}
	};

	std::cout << j.dump() << std::endl;
	std::cout << "///////////////////// Test 2" << std::endl;
	SDL_LogError(1, "SDL_Log JSON: %s", j.dump());


	std::cout << "///////////////////// Test 3" << std::endl;
	std::ofstream save_stream("test_uuid.json");
	sole::uuid test_id = sole::uuid1();
	save_stream << std::setw(4) << test_id << std::endl;
	std::cout << std::setw(4) << test_id.str() << std::endl;
	std::ifstream i("test_uuid.json");
	json j2;
	while (i >> j2)
	{
		try
		{
			std::cout << "Parse result: " << std::endl;
			std::cout << j2.dump(4) << std::endl;
		}
		catch (const std::exception& exc)
		{
			std::cerr << exc.what();
		}
		
	}
	







	return 0;
}