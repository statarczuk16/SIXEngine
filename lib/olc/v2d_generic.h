#pragma region license_and_help
/*
	olcPixelGameEngine.h
	+-------------------------------------------------------------+
	|           OneLoneCoder Pixel Game Engine v2.17              |
	|  "What do you need? Pixels... Lots of Pixels..." - javidx9  |
	+-------------------------------------------------------------+
	What is this?
	~~~~~~~~~~~~~
	olc::PixelGameEngine is a single file, cross platform graphics and userinput
	framework used for games, visualisations, algorithm exploration and learning.
	It was developed by YouTuber "javidx9" as an assistive tool for many of his
	videos. The goal of this project is to provide high speed graphics with
	minimal project setup complexity, to encourage new programmers, younger people,
	and anyone else that wants to make fun things.
	However, olc::PixelGameEngine is not a toy! It is a powerful and fast utility
	capable of delivering high resolution, high speed, high quality applications
	which behave the same way regardless of the operating system or platform.
	This file provides the core utility set of the olc::PixelGameEngine, including
	window creation, keyboard/mouse input, main game thread, timing, pixel drawing
	routines, image/sprite loading and drawing routines, and a bunch of utility
	types to make rapid development of games/visualisations possible.
	License (OLC-3)
	~~~~~~~~~~~~~~~
	Copyright 2018 - 2022 OneLoneCoder.com
	Redistribution and use in source and binary forms, with or without modification,
	are permitted provided that the following conditions are met:
	1. Redistributions or derivations of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.
	2. Redistributions or derivative works in binary form must reproduce the above
	copyright notice. This list of conditions and the following	disclaimer must be
	reproduced in the documentation and/or other materials provided with the distribution.
	3. Neither the name of the copyright holder nor the names of its contributors may
	be used to endorse or promote products derived from this software without specific
	prior written permission.
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS	"AS IS" AND ANY
	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
	OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
	SHALL THE COPYRIGHT	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
	INCIDENTAL,	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
	TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
	BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
	ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
	SUCH DAMAGE.
	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
				https://www.youtube.com/javidx9extra
	Discord:	https://discord.gg/WhwHUMV
	Twitter:	https://www.twitter.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Homepage:	https://www.onelonecoder.com
	Patreon:	https://www.patreon.com/javidx9
	Community:  https://community.onelonecoder.com
	Compiling in Linux
	~~~~~~~~~~~~~~~~~~
	You will need a modern C++ compiler, so update yours!
	To compile use the command:
	g++ -o YourProgName YourSource.cpp -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17
	On some Linux configurations, the frame rate is locked to the refresh
	rate of the monitor. This engine tries to unlock it but may not be
	able to, in which case try launching your program like this:
	vblank_mode=0 ./YourProgName
	Compiling in Code::Blocks on Windows
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Well I wont judge you, but make sure your Code::Blocks installation
	is really up to date - you may even consider updating your C++ toolchain
	to use MinGW32-W64.
	Guide for installing recent GCC for Windows:
	https://www.msys2.org/
	Guide for configuring code::blocks:
	https://solarianprogrammer.com/2019/11/05/install-gcc-windows/
	https://solarianprogrammer.com/2019/11/16/install-codeblocks-gcc-windows-build-c-cpp-fortran-programs/
	Add these libraries to "Linker Options":
	user32 gdi32 opengl32 gdiplus Shlwapi dwmapi stdc++fs
	Set these compiler options: -std=c++17
	Compiling on Mac - EXPERIMENTAL! PROBABLY HAS BUGS
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Yes yes, people use Macs for C++ programming! Who knew? Anyway, enough
	arguing, thanks to Mumflr the PGE is now supported on Mac. Now I know nothing
	about Mac, so if you need support, I suggest checking out the instructions
	here: https://github.com/MumflrFumperdink/olcPGEMac
	clang++ -arch x86_64 -std=c++17 -mmacosx-version-min=10.15 -Wall -framework OpenGL 
		-framework GLUT -framework Carbon -lpng YourSource.cpp -o YourProgName
	Compiling with Emscripten (New & Experimental)
	~~~~~~~~~~~~~~~~~~~~~~~~~
	Emscripten compiler will turn your awesome C++ PixelGameEngine project into WASM!
	This means you can run your application in teh browser, great for distributing
	and submission in to jams and things! It's a bit new at the moment. 
	em++ -std=c++17 -O2 -s ALLOW_MEMORY_GROWTH=1 -s MAX_WEBGL_VERSION=2 -s MIN_WEBGL_VERSION=2 -s USE_LIBPNG=1 ./YourSource.cpp -o pge.html
	Using stb_image.h
	~~~~~~~~~~~~~~~~~
	The PGE will load png images by default (with help from libpng on non-windows systems).
	However, the excellent "stb_image.h" can be used instead, supporting a variety of
	image formats, and has no library dependence - something we like at OLC studios ;)
	To use stb_image.h, make sure it's in your code base, and simply:
	#define OLC_IMAGE_STB
	Before including the olcPixelGameEngine.h header file. stb_image.h works on many systems
	and can be downloaded here: https://github.com/nothings/stb/blob/master/stb_image.h
	Multiple cpp file projects?
	~~~~~~~~~~~~~~~~~~~~~~~~~~~
	As a single header solution, the OLC_PGE_APPLICATION definition is used to
	insert the engine implementation at a project location of your choosing.
	The simplest way to setup multifile projects is to create a file called
	"olcPixelGameEngine.cpp" which includes the following:
	#define OLC_PGE_APPLICATION
	#include "olcPixelGameEngine.h"
	That's all it should include. You can also include PGEX includes and 
	defines in here too. With this in place, you dont need to 
	#define OLC_PGE_APPLICATION anywhere, and can simply include this 
	header file as an when you need to.
	Ports
	~~~~~
	olc::PixelGameEngine has been ported and tested with varying degrees of
	success to: WinXP, Win7, Win8, Win10, Various Linux, Raspberry Pi,
	Chromebook, Playstation Portable (PSP) and Nintendo Switch. If you are
	interested in the details of these ports, come and visit the Discord!
	Thanks
	~~~~~~
	I'd like to extend thanks to Ian McKay, Bispoo, Eremiell, slavka, Kwizatz77, gurkanctn, Phantim,
	IProgramInCPP, JackOJC, KrossX, Huhlig, Dragoneye, Appa, JustinRichardsMusic, SliceNDice, 
	dandistine,	Ralakus, Gorbit99, raoul, joshinils, benedani, Moros1138, Alexio, SaladinAkara 
	& MagetzUb for advice, ideas and testing, and I'd like to extend my appreciation to the
	250K YouTube followers,	80+ Patreons, 4.8K Twitch followers and 10K Discord server members 
	who give me	the motivation to keep going with all this :D
	Significant Contributors: @Moros1138, @SaladinAkara, @MaGetzUb, @slavka,
							  @Dragoneye, @Gorbit99, @dandistine & @Mumflr
	Special thanks to those who bring gifts!
	GnarGnarHead.......Domina
	Gorbit99...........Bastion, Ori & The Blind Forest, Terraria, Spelunky 2, Skully
	Marti Morta........Gris
	Danicron...........Terraria
	SaladinAkara.......Aseprite, Inside, Quern: Undying Thoughts, Outer Wilds
	AlterEgo...........Final Fantasy XII - The Zodiac Age
	SlicEnDicE.........Noita, Inside
	TGD................Voucher Gift
	Special thanks to my Patreons too - I wont name you on here, but I've
	certainly enjoyed my tea and flapjacks :D
	Author
	~~~~~~
	David Barr, aka javidx9, ©OneLoneCoder 2018, 2019, 2020, 2021, 2022
*/
#pragma endregion

#pragma region std_includes
// O------------------------------------------------------------------------------O
// | STANDARD INCLUDES                                                            |
// O------------------------------------------------------------------------------O
#include <cmath>
#include <cstdint>
#include <string>
#include <iostream>
#include <streambuf>
#include <sstream>
#include <chrono>
#include <vector>
#include <list>
#include <thread>
#include <atomic>
#include <fstream>
#include <map>
#include <functional>
#include <algorithm>
#include <array>
#include <cstring>
#pragma endregion

template <class T>
	struct v2d_generic
	{
		T x = 0;
		T y = 0;
		v2d_generic() : x(0), y(0) {}
		v2d_generic(T _x, T _y) : x(_x), y(_y) {}
		v2d_generic(const v2d_generic& v) : x(v.x), y(v.y) {}
		v2d_generic& operator=(const v2d_generic& v) = default;
		T mag() const { return T(std::sqrt(x * x + y * y)); }
		T mag2() const { return x * x + y * y; }
		v2d_generic  norm() const { T r = 1 / mag(); return v2d_generic(x * r, y * r); }
		v2d_generic  perp() const { return v2d_generic(-y, x); }
		v2d_generic  floor() const { return v2d_generic(std::floor(x), std::floor(y)); }
		v2d_generic  ceil() const { return v2d_generic(std::ceil(x), std::ceil(y)); }
		v2d_generic  max(const v2d_generic& v) const { return v2d_generic(std::max(x, v.x), std::max(y, v.y)); }
		v2d_generic  min(const v2d_generic& v) const { return v2d_generic(std::min(x, v.x), std::min(y, v.y)); }
		v2d_generic  cart() { return { std::cos(y) * x, std::sin(y) * x }; }
		v2d_generic  polar() { return { mag(), std::atan2(y, x) }; }
		T dot(const v2d_generic& rhs) const { return this->x * rhs.x + this->y * rhs.y; }
		T cross(const v2d_generic& rhs) const { return this->x * rhs.y - this->y * rhs.x; }
		v2d_generic  operator +  (const v2d_generic& rhs) const { return v2d_generic(this->x + rhs.x, this->y + rhs.y); }
		v2d_generic  operator -  (const v2d_generic& rhs) const { return v2d_generic(this->x - rhs.x, this->y - rhs.y); }
		v2d_generic  operator *  (const T& rhs)           const { return v2d_generic(this->x * rhs, this->y * rhs); }
		v2d_generic  operator *  (const v2d_generic& rhs) const { return v2d_generic(this->x * rhs.x, this->y * rhs.y); }
		v2d_generic  operator /  (const T& rhs)           const { return v2d_generic(this->x / rhs, this->y / rhs); }
		v2d_generic  operator /  (const v2d_generic& rhs) const { return v2d_generic(this->x / rhs.x, this->y / rhs.y); }
		v2d_generic& operator += (const v2d_generic& rhs) { this->x += rhs.x; this->y += rhs.y; return *this; }
		v2d_generic& operator -= (const v2d_generic& rhs) { this->x -= rhs.x; this->y -= rhs.y; return *this; }
		v2d_generic& operator *= (const T& rhs) { this->x *= rhs; this->y *= rhs; return *this; }
		v2d_generic& operator /= (const T& rhs) { this->x /= rhs; this->y /= rhs; return *this; }
		v2d_generic& operator *= (const v2d_generic& rhs) { this->x *= rhs.x; this->y *= rhs.y; return *this; }
		v2d_generic& operator /= (const v2d_generic& rhs) { this->x /= rhs.x; this->y /= rhs.y; return *this; }
		v2d_generic  operator +  () const { return { +x, +y }; }
		v2d_generic  operator -  () const { return { -x, -y }; }
		bool operator == (const v2d_generic& rhs) const { return (this->x == rhs.x && this->y == rhs.y); }
		bool operator != (const v2d_generic& rhs) const { return (this->x != rhs.x || this->y != rhs.y); }
		const std::string str() const { return std::string("(") + std::to_string(this->x) + "," + std::to_string(this->y) + ")"; }
		friend std::ostream& operator << (std::ostream& os, const v2d_generic& rhs) { os << rhs.str(); return os; }
		operator v2d_generic<int32_t>() const { return { static_cast<int32_t>(this->x), static_cast<int32_t>(this->y) }; }
		operator v2d_generic<float>() const { return { static_cast<float>(this->x), static_cast<float>(this->y) }; }
		operator v2d_generic<double>() const { return { static_cast<double>(this->x), static_cast<double>(this->y) }; }
	};

	// Note: joshinils has some good suggestions here, but they are complicated to implement at this moment, 
	// however they will appear in a future version of PGE
	template<class T> inline v2d_generic<T> operator * (const float& lhs, const v2d_generic<T>& rhs)
	{ return v2d_generic<T>((T)(lhs * (float)rhs.x), (T)(lhs * (float)rhs.y)); }
	template<class T> inline v2d_generic<T> operator * (const double& lhs, const v2d_generic<T>& rhs)
	{ return v2d_generic<T>((T)(lhs * (double)rhs.x), (T)(lhs * (double)rhs.y)); }
	template<class T> inline v2d_generic<T> operator * (const int& lhs, const v2d_generic<T>& rhs)
	{ return v2d_generic<T>((T)(lhs * (int)rhs.x), (T)(lhs * (int)rhs.y)); }
	template<class T> inline v2d_generic<T> operator / (const float& lhs, const v2d_generic<T>& rhs)
	{ return v2d_generic<T>((T)(lhs / (float)rhs.x), (T)(lhs / (float)rhs.y)); }
	template<class T> inline v2d_generic<T> operator / (const double& lhs, const v2d_generic<T>& rhs)
	{ return v2d_generic<T>((T)(lhs / (double)rhs.x), (T)(lhs / (double)rhs.y)); }
	template<class T> inline v2d_generic<T> operator / (const int& lhs, const v2d_generic<T>& rhs)
	{ return v2d_generic<T>((T)(lhs / (int)rhs.x), (T)(lhs / (int)rhs.y)); }

	// To stop dandistine crying...
	template<class T, class U> inline bool operator < (const v2d_generic<T>& lhs, const v2d_generic<U>& rhs)
	{ return lhs.y < rhs.y || (lhs.y == rhs.y && lhs.x < rhs.x); }
	template<class T, class U> inline bool operator > (const v2d_generic<T>& lhs, const v2d_generic<U>& rhs)
	{ return lhs.y > rhs.y || (lhs.y == rhs.y && lhs.x > rhs.x); }

	typedef v2d_generic<int32_t> vi2d;
	typedef v2d_generic<uint32_t> vu2d;
	typedef v2d_generic<float> vf2d;
	typedef v2d_generic<double> vd2d;