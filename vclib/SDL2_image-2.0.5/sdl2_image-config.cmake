message("Found sdl2_image-config.cmake at " ${CMAKE_CURRENT_LIST_DIR})

if(MSVC)
	message("Finding SDL_Image for Visual C++")
	set(SDL2_IMAGE_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include")
	# Support both 32 and 64 bit builds
	if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
	  message("64 Bit SDL_Image Found")
	  set(SDL2_IMAGE_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2_image.lib")
	else ()
	  message("32 Bit SDL_Image Found")
	  set(SDL2_IMAGE_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2_image.lib")
	endif ()
	message(${SDL2_IMAGE_LIBRARIES})

	
else ()
	message("Finding SDL_Image for GCC/MinGW")
	set(SDL2_IMAGE_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/x86_64-w64-mingw32/include/SDL2")
	set(SDL2_IMAGE_LIBDIR "${CMAKE_CURRENT_LIST_DIR}/x86_64-w64-mingw32/lib")
	set(SDL2_IMAGE_LIBRARIES "-L${SDL2_IMAGE_LIBDIR}  -lmingw32 -lSDL2main -lSDL2 -mwindows")
	SET(GCC_SDL2_IMAGE_COMPILER_FLAGS "-Wl,-Bstatic -lmingw32 -lSDL2main -lSDL2 -Wl,-Bdynamic -lws2_32 -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -lcomdlg32 -lhid -lsetupapi")
	set(SDL2_IMAGE_DLL_IMPORTED_LOCATION "${SDL2_IMAGE_LIBDIR}/libSDL2_image.dll")
	set(SDL2_IMAGE_DLL_IMPORTED_IMBLIB "${SDL2_IMAGE_LIBDIR}/libSDL2_image.dll.a")
	
endif ()

string(STRIP "${SDL2_IMAGE_LIBRARIES}" SDL2_IMAGE_LIBRARIES)
message("SDL2_Image Libraries: " ${SDL2_IMAGE_LIBRARIES})
message("SDL2_Image Include Dirs: " ${SDL2_IMAGE_LIBRARIES})

message("End sdl2_image-config.cmake")