
#include "sleep.hpp"


void milliSleep( unsigned long milliseconds )
{
#if defined ( __WIN32 )
        Sleep( milliseconds );
#else
        nanoSleep( milliseconds * 1000000 );
#endif
}

#ifndef __WIN32

#include <functional> // std::modulus

void microSleep( unsigned long microseconds )
{
        nanoSleep( microseconds * 1000 );
}

void nanoSleep ( unsigned long nanoseconds )
{
        std::modulus< unsigned long > modulus;
        unsigned long remainder = modulus( nanoseconds, 1000000000 );
        timespec pause;
        pause.tv_sec = nanoseconds / 1000000000;
        pause.tv_nsec = remainder;
        nanosleep( &pause, NULL );
}

#endif
