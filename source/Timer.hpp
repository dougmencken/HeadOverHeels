// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Timer_hpp_
#define Timer_hpp_

#include <sys/time.h>
#include <unistd.h>


/**
 * High precision timer for POSIX systems
 */

class Timer
{

public:

        Timer( ) ;

        virtual ~Timer( ) {}

        /**
         * Start timer
         */
        void go () ;

        /**
         * @return seconds since the timer was started
         */
        double getValue () ;

        /**
         * Stop timer
         */
        void stop () ;

        /**
         * Set the timer to the value of that another timer aka synchronise
         */
        void copyValueOf( const Timer & thatTimer ) ;
        void synchronizeWith( const Timer & anotherTimer ) {  copyValueOf( anotherTimer ) ;  }

private:

        // the moment when the chronometer (aka stopwatch or timer) was started
        timeval goTime ;

        // the moment when the stopwatch is stopped
        timeval stopTime ;

        struct timezone tz ;

        bool started ;

};

#endif
