cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
project(RiskTheSix)



set(VCLIB_PATH "${CMAKE_SOURCE_DIR}/vclib")

#The findsdl module needs a starting path to look for SDL2
#Try to determine the right path based on mingw/visual and 32/64bit
if(MSVC)
	set(SDL_HINT_END "include/")
	set(SDL_HINT_START ${VCLIB_PATH}/visual/*/)
elseif (MINGW)
	set(SDL_HINT_END "include/SDL2")
	if (${CMAKE_SIZEOF_VOID_P} MATCHES 8) #64 bit systems have length 8 words
		set(SDL_HINT_START ${VCLIB_PATH}/mingw/*/x86_64-w64-mingw32/)
	else()
		set(SDL_HINT_START ${VCLIB_PATH}/mingw/*/i686-w64-mingw32/)
	endif()
endif()
 
#put external libraries on path
list(APPEND CMAKE_PREFIX_PATH  "${CMAKE_SOURCE_DIR}/vclib/")
#put the cmake sdl tools on the path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)


#message("Hint Start:" ${SDL_HINT_START}) 
#message("Hint End:" ${SDL_HINT_END})

#Goal: SDL2_PATH should land in the folder with bin,include,etc. regardless of version.
# eg SIXEngine/vclib/visual/SDL2-2.0.14 or SIXEngine/vclib/mingw/SDL2-2.0.14/x86_64-w64-mingw32
# if have a path called this/is/VERSION1.2/path/to/stuff.dll
# find_path(NEW_PATH, NAMES, path/to/stuff.dll, PATHS, this/is/*/)
# will return this/is/VERSION1.2 or this/is/VERSION1.3 etc. (accounts for differing versions)
find_path(SDL2_PATH_ NAMES ${SDL_HINT_END}/SDL.h PATHS ${SDL_HINT_START})

message("SDL2_PATH_:"  ${SDL2_PATH_})
set(SDL2_PATH ${SDL2_PATH_} CACHE BOOL "" FORCE) #Override the cache variable SDL2_PATH which affects operation of find_package(SDL2)
message("SDL2_PATH: " ${SDL2_PATH})
find_package(SDL2 REQUIRED)

find_path(SDL2_IMAGE_PATH_ NAMES ${SDL_HINT_END}/SDL_Image.h PATHS ${SDL_HINT_START})
message("SDL2_IMAGE_PATH_: " ${SDL2_IMAGE_PATH_})
set(SDL2_IMAGE_PATH  ${SDL2_IMAGE_PATH_} CACHE BOOL "" FORCE)
message("SDL2_IMAGE_PATH: " ${SDL2_IMAGE_PATH})
find_package(SDL2_image REQUIRED)  

find_path(SDL2_TTF_PATH_ NAMES ${SDL_HINT_END}/SDL_TTF.h PATHS ${SDL_HINT_START})
message("SDL2_TTF_PATH_: " ${SDL2_TTF_PATH_})
set(SDL2_TTF_PATH  ${SDL2_TTF_PATH_} CACHE BOOL "" FORCE)
message("SDL2_TTF_PATH: " ${SDL2_TTF_PATH})
find_package(SDL2_ttf REQUIRED) 

 
 ######### SXNGN
#include the SXNGN Sources (game engine)
file(GLOB_RECURSE SXNGN_SOURCES
	${CMAKE_SOURCE_DIR}/lib/SXNGN/headers/*.h
	${CMAKE_SOURCE_DIR}/lib/SXNGN/cpp/*.hpp
	${CMAKE_SOURCE_DIR}/lib/SXNGN/cpp/*.cpp
	) 
	
message("SXNGN Sources: "  ${SXNGN_SOURCES})
include_directories(${CMAKE_SOURCE_DIR}/lib/SXNGN/headers)   

 ######### KISS (GUI)
 file(GLOB_RECURSE KISS_SOURCES
	${CMAKE_SOURCE_DIR}/lib/KISS/kiss_sdl/*.h
	${CMAKE_SOURCE_DIR}/lib/KISS/kiss_sdl/*.c 
	${CMAKE_SOURCE_DIR}/lib/KISS/kiss_sdl/*.cpp 
	)
	
message("KISS Sources: "  ${KISS_SOURCES}) 
include_directories(${CMAKE_SOURCE_DIR}/lib/KISS/kiss_sdl/)     
  
  ######### NLOHMANN JSON (JSON C++ Library)
 file(GLOB_RECURSE JSON_SOURCES
	${CMAKE_SOURCE_DIR}/lib/json/nlohmann/*.hpp
	)
	
message("JSON Sources: "  ${JSON_SOURCES}) 
include_directories(${CMAKE_SOURCE_DIR}/lib/json/)	 
 
  ######### Cereal (Serialization Library)
# file(GLOB_RECURSE CEREAL_SOURCES
#	${CMAKE_SOURCE_DIR}/lib/cereal/*.hpp
#	)
#	
#message("Cereal Sources: "  ${CEREAL_SOURCES}) 
#include_directories(${CMAKE_SOURCE_DIR}/lib/cereal/)	
 
#add an executable for anything that will turn into an exe or whatever other executable format
add_executable(HoplonSimulator "src/apps/HoplonSimulator.cpp" ${SXNGN_SOURCES} ${KISS_SOURCES} ${JSON_SOURCES}  "lib/SXNGN/headers/ECS/Core/StateManager.hpp" "lib/SXNGN/headers/ECS/Systems/MovementSystem.hpp" "lib/SXNGN/cpp/ECS/Systems/MovementSystem.cpp" "lib/SXNGN/headers/ECS/Components/Tile.hpp"  "lib/SXNGN/headers/ECS/Components/Renderable.hpp" "lib/SXNGN/cpp/ECS/Components/SpriteFactory.cpp" "lib/SXNGN/headers/ECS/Components/StateIndicatorComponent.hpp" "lib/SXNGN/headers/ECS/Components/UI.hpp" "lib/SXNGN/cpp/ECS/Components/UI.cpp" "lib/SXNGN/headers/ECS/Utilities/Game_Utils.hpp")
target_link_libraries(HoplonSimulator SDL2::Main SDL2::Image SDL2::TTF)

add_executable(ECS_Demo "src/apps/ECS_Demo.cpp" ${SXNGN_SOURCES} ${KISS_SOURCES}  ${JSON_SOURCES}  "lib/SXNGN/headers/ECS/Core/StateManager.hpp" "lib/SXNGN/headers/ECS/Systems/MovementSystem.hpp" "lib/SXNGN/cpp/ECS/Systems/MovementSystem.cpp" "lib/SXNGN/headers/ECS/Components/Tile.hpp"  "lib/SXNGN/headers/ECS/Components/Renderable.hpp" "lib/SXNGN/cpp/ECS/Components/SpriteFactory.cpp" "lib/SXNGN/headers/ECS/Components/StateIndicatorComponent.hpp" "lib/SXNGN/headers/ECS/Components/UI.hpp" "lib/SXNGN/cpp/ECS/Components/UI.cpp" "lib/SXNGN/headers/ECS/Utilities/Game_Utils.hpp")
target_link_libraries(ECS_Demo SDL2::Main SDL2::Image SDL2::TTF)
 
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

