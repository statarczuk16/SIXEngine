#pragma once

#include <SDL_stdinc.h>
#include <SDL.h>
//The tile
namespace SXNGN {

	class Timer
	{
	public:
		//Initializes variables
		Timer();

		//The various clock actions
		void start();
		void stop();
		void pause();
		void unpause();

		//Gets the timer's time since start
		float getMSSinceTimerStart();

		//Gets the timer's time since last check
		float getMSSinceLastCheck();

		//Checks the status of the timer
		bool isStarted();
		bool isPaused();

	private:
		//The clock time when the timer started
		Uint64 mStartTicks;

		//The ticks stored when the timer was paused
		Uint64 mPausedTicks;

		//Ticks when last checked
		Uint64 mLastCheckTicks;

		//The timer status
		bool mPaused;
		bool mStarted;
	};
}