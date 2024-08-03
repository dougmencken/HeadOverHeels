#include "Timer.hpp"


Timer::Timer()
{
        goTime.tv_sec = 0 ;
        goTime.tv_usec = 0 ;
        started = false ;
}

void Timer::go()
{
        gettimeofday( &goTime, &tz );
        started = true ;
}

double Timer::getValue()
{
        if ( ! this->started ) {
                go() ;
                return 0.0 ;
        }

        timeval currentTime ;
        gettimeofday( &currentTime, &tz );

        double go = double( goTime.tv_sec ) + double( goTime.tv_usec ) / 1000000 ;
        double current = double( currentTime.tv_sec ) + double( currentTime.tv_usec ) / 1000000 ;

        return current - go ;
}

void Timer::copyValueOf( const Timer & thatTimer )
{
        if ( thatTimer.started ) {
                this->goTime.tv_sec = thatTimer.goTime.tv_sec ;
                this->goTime.tv_usec = thatTimer.goTime.tv_usec ;

                this->started = true ;
        }
}
