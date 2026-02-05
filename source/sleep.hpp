// The free and open source remake of Head over Heels
//
// Copyright © 2026 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef sleep_hpp_
#define sleep_hpp_

#include <time.h>

namespace somn {

/**
 * Pause subprocess for a given period of time in milliseconds
 */
void milliSleep ( unsigned long miliseconds );

/**
 * Pause subprocess for a given period of time in microseconds
 */
void microSleep ( unsigned long microseconds );

/**
 * Pause subprocess for a given period of time in nanoseconds.
 * But implementation may sleep longer than given period
 */
void nanoSleep ( unsigned long nanoseconds );

}

#endif
