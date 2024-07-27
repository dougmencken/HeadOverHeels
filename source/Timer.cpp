#include "Timer.hpp"


Timer::Timer()
{
        goTime.tv_sec = 0 ;
        goTime.tv_usec = 0 ;
        stopTime.tv_sec = 0 ;
        stopTime.tv_usec = 0 ;

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

        gettimeofday( &stopTime, &tz );

        double go = double( goTime.tv_sec ) + double( goTime.tv_usec ) / 1000000 ;
        double stop = double( stopTime.tv_sec ) + double( stopTime.tv_usec ) / 1000000 ;

        return stop - go ;
}

void Timer::stop()
{
        gettimeofday( &stopTime, &tz );
}

void Timer::copyValueOf( const Timer & thatTimer )
{
        if ( thatTimer.started ) {
                this->goTime.tv_sec = thatTimer.goTime.tv_sec ;
                this->goTime.tv_usec = thatTimer.goTime.tv_usec ;
                this->stopTime.tv_sec = thatTimer.stopTime.tv_sec ;
                this->stopTime.tv_usec = thatTimer.stopTime.tv_usec ;

                this->started = true ;
        }
}
