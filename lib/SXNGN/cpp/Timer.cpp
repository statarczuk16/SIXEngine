#include <Timer.h>
#include <iostream>


SXNGN::Timer::Timer()
{
    //Initialize the variables
    mStartTicks = 0;
    mPausedTicks = 0;

    mPaused = false;
    mStarted = false;
}

void SXNGN::Timer::start()
{
    //Start the timer
    mStarted = true;

    //Unpause the timer
    mPaused = false;

    //Get the current clock time
    mStartTicks = SDL_GetPerformanceCounter();
    mPausedTicks = 0;
    mLastCheckTicks = mStartTicks;
}

void SXNGN::Timer::stop()
{
    //Stop the timer
    mStarted = false;

    //Unpause the timer
    mPaused = false;

    //Clear tick variables
    mStartTicks = 0;
    mPausedTicks = 0;
}

void SXNGN::Timer::pause()
{
    //If the timer is running and isn't already paused
    if (mStarted && !mPaused)
    {
        //Pause the timer
        mPaused = true;

        //Calculate the paused ticks
        mPausedTicks = SDL_GetPerformanceCounter() - mStartTicks;
        mStartTicks = 0;
    }
}

void SXNGN::Timer::unpause()
{
    //If the timer is running and paused
    if (mStarted && mPaused)
    {
        //Unpause the timer
        mPaused = false;

        //Reset the starting ticks
        mStartTicks = SDL_GetPerformanceCounter() - mPausedTicks;

        //Reset the paused ticks
        mPausedTicks = 0;
    }
}

double SXNGN::Timer::getMSSinceTimerStart()
{
    //The actual timer time
    Uint64 ticks_since_start = 0.0;

    //If the timer is running
    if (mStarted)
    {
        //If the timer is paused
        if (mPaused)
        {
            //Return the number of ticks when the timer was paused
            ticks_since_start = mPausedTicks;
        }
        else
        {
            //Return the current time minus the start time
            ticks_since_start = SDL_GetPerformanceCounter() - mStartTicks;
            
        }
    }
    double seconds_since_start = (double)ticks_since_start / (double)SDL_GetPerformanceFrequency();
    return (double) seconds_since_start;
}

double SXNGN::Timer::getMSSinceLastCheck()
{
    //The actual timer time
    Uint64 ticks_since_check = 0.0;
    Uint64 new_check_time = 0.0;

    //If the timer is running
    if (mStarted)
    {
        //If the timer is paused
        if (mPaused)
        {
            //Return time from the last check time until the pause time
            ticks_since_check = mPausedTicks - mLastCheckTicks;
            //new check time is now the pause time 
            new_check_time = mPausedTicks;
        }
        else
        {
            //Return time from last check until now
            new_check_time = SDL_GetPerformanceCounter();
            //new check time is now
            ticks_since_check = new_check_time - mLastCheckTicks;
        }
    }
    mLastCheckTicks = new_check_time;
    double seconds_since_start = (double)ticks_since_check / (double)SDL_GetPerformanceFrequency();
    //std::cout << "Accumulated: " << seconds_since_start << std::endl;
    return (double)seconds_since_start * 1000.0;
}

bool SXNGN::Timer::isStarted()
{
    //Timer is running and paused or unpaused
    return mStarted;
}

bool SXNGN::Timer::isPaused()
{
    //Timer is running and paused
    return mPaused && mStarted;
}
