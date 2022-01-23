#include <nlohmann/json.hpp>
#include <iostream>
#include <SDL.h>

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

	return 0;
}