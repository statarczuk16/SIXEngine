Copyright Notice:
-----------------
© 2021 Stephen Tatarczuk
The files within this repository are copyrighted by Stephen Tatarczuk unless otherwise noted
and may not be redistributed without written permission.

# About

This is a C++ game engine using SDL2 and maybe someday OpenGL. It is geared toward top-down, tile based games. 

# Install/Use

Dependencies:
SDL2
SDL2_TTF
SDL2_Image
Boost
kiss_sdl (https://github.com/actsl/kiss_sdl)

Cmake will look for these libraries in the vc_lib directory if not elsewhere wherever you put libraries.
There must be a config.cmake for each library, which are not usually distributed with the cmake library downloads. Therefore, take the ones from SXNGN/docs and place into the library downloads like so:

Example showing how vclib should be set up. This is where cmakelists.txt will look for the libraries.

<pre>
└── SXNGN
│   │   README.md (you are here)
│      
└───vclib
    │   
	└──	SDL2_image-2.0.5
			sdl2_image-config.cmake
    └──	SDL2_ttf-2.0.15
			sdl2_ttf-config.cmake
	└──	SDL2_2.0.14
			sdl2-config.cmake
</pre>

			
To use in an IDE, open this directory in a C++ IDE that supports CMake such as Visual Studio or CLion and build as normal. 

To build in terminal, create a "build" directory at the same level as this readme.
"cd build"
"cmake .."
"make" (or whatever you are using to build - the point of cmake is that you should be able to use make or visual studio or whatever to compile)

Note: I use Visual Studio 2019 on Windows 10, so it has only been tested there.

Don't forget that the .dll files in each library used must be either next to the final .exe you intend to run or on your system path. 

# Adding an application

Note the presence of the src/apps folder. 

This contains programs with a main loop, applications, programs that will become an .exe, etc.

cmakelists.txt must be told about any new main applications directly. Note the format of the "add_executable" and "target_link_libraries" in cmakelists.txt and copy accordingly for any new application.





