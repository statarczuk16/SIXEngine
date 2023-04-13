cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
project(RiskTheSix)
set( CMAKE_MESSAGE_LOG_LEVEL "STATUS" )
add_definitions(-DNOMINMAX) #disable stupid windows.h thing intefering with std::min std::max

set(CMAKE_BUILD_TYPE Debug) 


set(VCLIB_PATH "${CMAKE_SOURCE_DIR}/vclib")
#put external libraries on path
list(APPEND CMAKE_PREFIX_PATH  "${CMAKE_SOURCE_DIR}/vclib/") 
#put the cmake sdl tools on the path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)  

#To help find_package find SDL2 when on Windows and libraries are in vclib folder.
if(MSVC)  
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Zi")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG") 
	# Enable AddressSanitizer
	#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address")
	#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")

	# Set linker flags for AddressSanitizer
	#set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address")
	message("Setting hints for MSVC")
	set(SDL_HINT_END "include/")
	set(SDL_HINT_START ${VCLIB_PATH}/visual/*/)   
	#add_compile_options(/WX)
    #add_link_options(/WX)
elseif (MINGW)
	message("Setting hints for MINGW")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
	#add_compile_options(-Wall -Wextra -pedantic -Werror -Wl,--fatal-warnings)
	set(SDL_HINT_END "include/SDL2")
	if (${CMAKE_SIZEOF_VOID_P} MATCHES 8) #64 bit systems have length 8 words
		set(SDL_HINT_START ${VCLIB_PATH}/mingw/*/x86_64-w64-mingw32/)
	else()
		set(SDL_HINT_START ${VCLIB_PATH}/mingw/*/i686-w64-mingw32/) 
	endif()
else()
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
	message("Linux")   
endif()

if(MSVC OR MINGW)
	message("Setting hints for Windows")
	message("Hint Start:" ${SDL_HINT_START}) 
	message("Hint End:" ${SDL_HINT_END})
	#Goal: SDL2_PATH should land in the folder with bin,include,etc. regardless of version.
	# eg SIXEngine/vclib/visual/SDL2-2.0.14 or SIXEngine/vclib/mingw/SDL2-2.0.14/x86_64-w64-mingw32
	# if have a path called this/is/VERSION1.2/path/to/stuff.dll
	# find_path(NEW_PATH, NAMES, path/to/stuff.dll, PATHS, this/is/*/)
	# will return this/is/VERSION1.2 or this/is/VERSION1.3 etc. (accounts for differing versions)
	find_path(SDL2_PATH_ NAMES ${SDL_HINT_END}/SDL.h PATHS ${SDL_HINT_START})
	message("SDL2_PATH_:"  ${SDL2_PATH_})
	set(SDL2_PATH ${SDL2_PATH_} CACHE BOOL "" FORCE) #Override the cache variable SDL2_PATH which affects operation of find_package(SDL2)
	message("SDL2_PATH: " ${SDL2_PATH})

	find_path(SDL2_IMAGE_PATH_ NAMES ${SDL_HINT_END}/SDL_image.h PATHS ${SDL_HINT_START})
	message("SDL2_IMAGE_PATH_: " ${SDL2_IMAGE_PATH_})
	set(SDL2_IMAGE_PATH  ${SDL2_IMAGE_PATH_} CACHE BOOL "" FORCE)
	message("SDL2_IMAGE_PATH: " ${SDL2_IMAGE_PATH})

	find_path(SDL2_TTF_PATH_ NAMES ${SDL_HINT_END}/SDL_ttf.h PATHS ${SDL_HINT_START})
	message("SDL2_TTF_PATH_: " ${SDL2_TTF_PATH_})
	set(SDL2_TTF_PATH  ${SDL2_TTF_PATH_} CACHE BOOL "" FORCE)
	message("SDL2_TTF_PATH: " ${SDL2_TTF_PATH})
endif()

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED) 
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

 ######### UUID (UUID C++ Library)
 file(GLOB_RECURSE UUID_SOURCES
	${CMAKE_SOURCE_DIR}/lib/sole-master/*.hpp
	)
	
message("UUID Sources: "  ${UUID_SOURCES}) 
include_directories(${CMAKE_SOURCE_DIR}/lib/sole-master/)	  


 ######### SDL_FontCache (text rendering)
 file(GLOB_RECURSE SDL_FONTCACHE_SOURCES
	${CMAKE_SOURCE_DIR}/lib/SDL_FontCache/*.h
	${CMAKE_SOURCE_DIR}/lib/SDL_FontCache/*.c
	)
	
message("SDL_FontCache Sources: "  ${SDL_FONTCACHE_SOURCES}) 
include_directories(${CMAKE_SOURCE_DIR}/lib/SDL_FontCache/)

 ######### Pixel Game Engine Vector (vector)
 file(GLOB_RECURSE OLC_SOURCES
	${CMAKE_SOURCE_DIR}/lib/olc/*.h
	)
	
message("Olc Sources: "  ${OLC_SOURCES}) 
include_directories(${CMAKE_SOURCE_DIR}/lib/olc/)

 
#add an executable for anything that will turn into an exe or whatever other executable format  
add_executable(json_tests "src/tests/json_tests.cpp" ${SXNGN_SOURCES} ${OLC_SOURCES} ${KISS_SOURCES} ${JSON_SOURCES}  ${UUID_SOURCES} ${SDL_FONTCACHE_SOURCES})
target_link_libraries(json_tests SDL2::Main SDL2::Image SDL2::TTF)  

add_executable(ApocalypseHighwayTrader "src/apps/ECS_Demo.cpp" ${SXNGN_SOURCES} ${OLC_SOURCES} ${KISS_SOURCES}  ${JSON_SOURCES}  ${UUID_SOURCES} ${SDL_FONTCACHE_SOURCES})
target_link_libraries(ApocalypseHighwayTrader SDL2::Main SDL2::Image SDL2::TTF) 

add_executable(system_tests "src/tests/system_tests.cpp" ${SXNGN_SOURCES} ${OLC_SOURCES} ${KISS_SOURCES}  ${JSON_SOURCES}  ${UUID_SOURCES} ${SDL_FONTCACHE_SOURCES})
target_link_libraries(system_tests SDL2::Main SDL2::Image SDL2::TTF)

add_executable(unit_tests "src/tests/unit_tests.cpp" ${SXNGN_SOURCES} ${OLC_SOURCES} ${KISS_SOURCES}  ${JSON_SOURCES}  ${UUID_SOURCES} ${SDL_FONTCACHE_SOURCES})
target_link_libraries(unit_tests SDL2::Main SDL2::Image SDL2::TTF) 

add_executable(kiss_example_1 "lib/KISS/kiss_example1.c" ${SXNGN_SOURCES} ${OLC_SOURCES} ${KISS_SOURCES}  ${JSON_SOURCES}  ${UUID_SOURCES} ${SDL_FONTCACHE_SOURCES})
target_link_libraries(kiss_example_1 SDL2::Main SDL2::Image SDL2::TTF) 
 
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

