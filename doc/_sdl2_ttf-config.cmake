set(SDL2_TTF_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include")

# Support both 32 and 64 bit builds
if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
  message("64 Bit SDL_TTF Found")
  set(SDL2_TTF_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2_ttf.lib")
else ()
  message("32 Bit SDL_TTF Found")
  set(SDL2_TTF_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2_ttf.lib")
endif ()
message(${SDL2_TTF_LIBRARIES})

string(STRIP "${SDL2_TTF_LIBRARIES}" SDL2_TTF_LIBRARIES)