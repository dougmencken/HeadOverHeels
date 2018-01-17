
#ifdef __WIN32

#include "TimerWindows.hpp"


Timer::Timer()
{
        frequency.QuadPart = 0;
        startTime.QuadPart = 0;
        stopTime.QuadPart = 0;
        period.QuadPart = 0;
}

bool Timer::go()
{
        QueryPerformanceCounter( &startTime );
        if ( ! QueryPerformanceFrequency( &frequency ) ) return false;

        frequency.QuadPart /= 1000;     // to milliseconds
        return true;
}

double Timer::getValue()
{
        LARGE_INTEGER timer;
        QueryPerformanceCounter( &timer );

        if ( stopTime.QuadPart != 0 )
        {
                return double( ( stopTime.QuadPart - startTime.QuadPart ) / frequency.QuadPart ) / 1000.0;
        }

        return double( ( timer.QuadPart - period.QuadPart - startTime.QuadPart ) / frequency.QuadPart ) / 1000.0;
}

void Timer::reset()
{
        LARGE_INTEGER timer, excess;

        QueryPerformanceCounter( &timer );
        excess.QuadPart = ( timer.QuadPart - startTime.QuadPart ) % frequency.QuadPart;
        QueryPerformanceCounter( &startTime );
        startTime.QuadPart -= excess.QuadPart;
        period.QuadPart = 0;
}

void Timer::stop()
{
        LARGE_INTEGER timer;
        QueryPerformanceCounter( &timer );

        stopTime.QuadPart = timer.QuadPart - period.QuadPart;
}

void Timer::restart()
{
        LARGE_INTEGER timer;
        QueryPerformanceCounter( &timer );

        period.QuadPart = ( timer.QuadPart - stopTime.QuadPart );
        stopTime.QuadPart = 0;
}

#endif
