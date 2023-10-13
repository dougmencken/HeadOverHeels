
#ifndef __WIN32

#include "TimerPOSIX.hpp"


Timer::Timer()
{
        trestart.tv_sec = 0;
        trestart.tv_usec = 0;
        tstop.tv_sec = 0;
        tstop.tv_usec = 0;
}

Timer::~Timer()
{

}

void Timer::go()
{
        gettimeofday( &trestart, &tz );
}

double Timer::getValue()
{
        gettimeofday( &tstop, &tz );

        double t1 = double( trestart.tv_sec ) + double( trestart.tv_usec ) / 1000000;
        double t2 = double( tstop.tv_sec ) + double( tstop.tv_usec ) / 1000000;

        return t2 - t1;
}

void Timer::reset()
{
        gettimeofday( &trestart, &tz );
}

void Timer::stop()
{
        gettimeofday( &tstop, &tz );
}

void Timer::restart()
{
        gettimeofday( &tstop, &tz );
}

#endif
