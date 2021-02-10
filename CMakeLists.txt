cmake_minimum_required(VERSION 3.7)

project(SDL2Test)

list(APPEND CMAKE_PREFIX_PATH  "${CMAKE_SOURCE_DIR}/../vclib/")

find_package(SDL2 CONFIG REQUIRED)

find_package(SDL2_Image CONFIG REQUIRED)

find_package(Boost REQUIRED COMPONENTS filesystem)

message("SDL2_LIBRARIES: "  ${SDL2_LIBRARIES})
message("SDL2_INCLUDE: "  ${SDL2_INCLUDE_DIRS})
message("SDL2_IMAGE_LIBRARIES: "  ${SDL2_IMAGE_LIBRARIES})
message("SDL2_IMAGE_INCLUDE: "  ${SDL2_IMAGE_INCLUDE_DIRS})
message("Boost Found: " ${Boost_FOUND})
message("Boost_INCLUDE_DIR: " ${Boost_INCLUDE_DIRS})
message("Boost_LIBRARY_DIRS: " ${Boost_LIBRARY_DIRS})
message("Boost_LIBRARIES: " ${Boost_LIBRARIES})

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

#link_directories(${SDL2_LIBRARIES})

file(GLOB_RECURSE SOURCES
	${CMAKE_SOURCE_DIR}/src/*.h
	${CMAKE_SOURCE_DIR}/src/*.cpp
	)
	
message("Sources: "  ${SOURCES})

include_directories(${CMAKE_SOURCE_DIR}/src/headers)

add_executable(SDL2Test ${SOURCES} "src/cpp/Dot.cpp" "src/headers/Dot.h" "src/cpp/Tile.cpp" "src/headers/Tile.h" "src/cpp/Collision.cpp" "src/headers/Collision.h" "src/headers/Texture.h")
target_link_libraries(SDL2Test ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} Boost::filesystem)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


#Copy DLLs to output
#add_custom_command(TARGET SDL2Test POST_BUILD        # Adds a post-build event to SDL2Test
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
#        "${SDL2_LIBRARIES}../.dll)"      # <--this is in-file
#        $<CMAKE_BINARY_DIR:SDL2Test)                 # <--this is out-file path