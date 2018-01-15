// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef LinuxHPC_hpp_
#define LinuxHPC_hpp_

#include <sys/time.h>
#include <unistd.h>

/**
 * High precision timer for POSIX systems
 */

class HPC
{

public:

        HPC( ) ;

        virtual ~HPC( ) ;

       /**
        * Start timer
        */
        void start () ;

       /**
        * Return milliseconds since timer was started
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

protected:

       /**
        * Moment in which chronometer was started
        */
        timeval trestart ;

       /**
        * Moment in which stopwatch is stopped
        */
        timeval tstop ;

       /**
        * Huso horario
        */
        struct timezone tz ;

};

#endif
