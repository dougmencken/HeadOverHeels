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

        virtual ~Timer( ) ;

        /**
         * Start timer
         */
        void go () ;

        /**
         * @return seconds since the timer was started
         */
        double getValue () ;

        /**
         * Reset timer
         */
        void reset () ;

        /**
         * Stop timer
         */
        void stop () ;

        /**
         * Restart timer
         */
        void restart () ;

private:

        // the moment when the chronometer (aka stopwatch or timer) was started
        timeval trestart ;

        // the moment when the stopwatch is stopped
        timeval tstop ;

        struct timezone tz ;

};

#endif
