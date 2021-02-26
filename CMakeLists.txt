cmake_minimum_required(VERSION 3.7)

project(RiskTheSix)

list(APPEND CMAKE_PREFIX_PATH  "${CMAKE_SOURCE_DIR}/../vclib/")

find_package(SDL2 CONFIG REQUIRED)

find_package(SDL2_Image CONFIG REQUIRED)

find_package(SDL2_ttf CONFIG REQUIRED)

find_package(Boost REQUIRED COMPONENTS filesystem)

message("SDL2_LIBRARIES: "  ${SDL2_LIBRARIES})
message("SDL2_INCLUDE: "  ${SDL2_INCLUDE_DIRS})
message("SDL2_IMAGE_LIBRARIES: "  ${SDL2_IMAGE_LIBRARIES})
message("SDL2_IMAGE_INCLUDE: "  ${SDL2_IMAGE_INCLUDE_DIRS})
message("SDL2_TTF_LIBRARIES: "  ${SDL2_TTF_LIBRARIES})
message("SDL2_TTF_INCLUDE: "  ${SDL2_TTF_INCLUDE_DIRS})

message("Boost Found: " ${Boost_FOUND})
message("Boost_INCLUDE_DIR: " ${Boost_INCLUDE_DIRS})
message("Boost_LIBRARY_DIRS: " ${Boost_LIBRARY_DIRS})
message("Boost_LIBRARIES: " ${Boost_LIBRARIES})

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})

#link_directories(${SDL2_LIBRARIES})

file(GLOB_RECURSE SXNGN_SOURCES
	${CMAKE_SOURCE_DIR}/lib/SXNGN/headers/*.h
	${CMAKE_SOURCE_DIR}/lib/SXNGN/cpp/*.cpp
	)
	
message("Sources: "  ${SXNGN_SOURCES})

include_directories(${CMAKE_SOURCE_DIR}/lib/SXNGN/headers)

#include_directories(${CMAKE_SOURCE_DIR}/src/headers)
add_executable(LevelEditor "src/apps/LevelEditor.cpp" ${SXNGN_SOURCES} "lib/SXNGN/cpp/World/Room.cpp" "lib/SXNGN/headers/State/BaseGameState.h" "lib/SXNGN/cpp/State/BaseGameState.cpp" "lib/SXNGN/headers/Camera.h" "lib/SXNGN/cpp/Camera.cpp" "lib/SXNGN/headers/Database.h" "lib/SXNGN/cpp/Database.cpp" "lib/SXNGN/cpp/Timer.cpp")
target_link_libraries(LevelEditor ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES}  ${SDL2_TTF_LIBRARIES} Boost::filesystem)

add_executable(HoplonSimulator "src/apps/HoplonSimulator.cpp" ${SXNGN_SOURCES} "lib/SXNGN/cpp/World/Room.cpp" "lib/SXNGN/headers/State/BaseGameState.h" "lib/SXNGN/cpp/State/BaseGameState.cpp" "lib/SXNGN/headers/Camera.h" "lib/SXNGN/cpp/Camera.cpp" "lib/SXNGN/headers/Database.h" "lib/SXNGN/cpp/Database.cpp" "lib/SXNGN/cpp/Timer.cpp")
target_link_libraries(HoplonSimulator ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES} Boost::filesystem)



set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


#Copy DLLs to output
#add_custom_command(TARGET SDL2Test POST_BUILD        # Adds a post-build event to SDL2Test
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
#        "${SDL2_LIBRARIES}../.dll)"      # <--this is in-file
#        $<CMAKE_BINARY_DIR:SDL2Test)                 # <--this is out-file path