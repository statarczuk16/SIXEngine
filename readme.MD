Copyright Notice:
-----------------
© 2021 Stephen Tatarczuk
The files within this repository are copyrighted by Stephen Tatarczuk unless otherwise noted
and may not be redistributed without written permission.

# About

This is an Entity Component System Architecture game engine aimed at 2D tile based games. 

# Install/Use

Dependencies:
SDL2
SDL2_TTF
SDL2_Image
kiss_sdl (https://github.com/actsl/kiss_sdl) (Modified for this project, version included must be used)
C++ 17

These dependencies are all included in the repo. You will have better luck using these ones then whatever is
installed on your system. 

The vclib.../bin folder corresponding to your compiler/architecture must be added to the path
else you will get "no such sdl.dll" type errors.

<pre>
└── README.md (you are here)
│ 
└── SRC
│   │   
│      
└───vclib
    │   
	└──	visual 
		└── SDL2_image-2.0.5 
		└── SDL2_ttf-2.0.15
		└── SDL2_2.0.14
	
	└──	mingw 
		└── SDL2_image-2.0.5
		└── SDL2_ttf-2.0.15
		└── SDL2_2.0.14

</pre>

## Visual Studio	
* Simply download the repo and open the parent folder (the one with this readme) with "open folder". 
* Provided you have the cmake plugin, you should just have to build as normal.
* All the SDL DLLs must be on your path or copy/pasted to the generated exe's location
## Clion
* Same as above, though I have not tested it.
## Codeblocks/Other MinGW/GCC IDE
* You must have a C++17 mingw install. You can likely find a guide online for getting Codeblocks set up for C++17 mingw.
** Make sure the C++17 compiler flags are set. std::filesystem is the main C++17 dependency in this project.
* Download Cmake gui
* Point Cmake gui to this folder as the source
* Create a build folder in this folder and point Cmake to it as the build destination
* Click "Configure" 
* When Cmake asks for compiler information, tell it to use Mingw Makefiles or Codeblocks Mingw Makefiles
** Not sure if there's a difference between the two
* Cmake should find sdl, sdl-image, and sdl-ttf in the vclib directory. 
** If you have sdl2 in your mingw directory already, it may find it there instead.
*** This may introduce "SDL.h" vs "SDL2/SDL.h" C++ include errors. 
*** You can solve this by removing your mingw SDL2 folder so cmake is forced to use the SDL2 in vclib
*** Or you can hard-code SDL2_PATH in cmakelists.txt to the bin,include,lib, etc area of the SDL2 area in vclib.
* Click "Generate" -> will generate makefiles and a codeblocks project (CBP)
* Open Codeblocks and open the CPB
* Build/run as normal
* All the SDL DLLs must be on your path or copy/pasted to the generated exe's location
## MINGW/Visual Terminal
* To build in terminal, create a "build" directory at the same level as this readme.
* "cd build"
* "cmake .."
* "make" 
* Run the resulting application

# Code a new application

Note the presence of the src/apps folder. 

This contains programs with a main loop, applications, programs that will become an .exe, etc.

cmakelists.txt must be told about any new main applications directly. Note the format of the "add_executable" and "target_link_libraries" in cmakelists.txt and copy accordingly for any new application.





