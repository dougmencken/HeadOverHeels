
#include "sleep.hpp"

#include <functional> // std::modulus


namespace somn {

void milliSleep( unsigned long milliseconds )
{
        nanoSleep( milliseconds * 1000000 );
}

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

}
