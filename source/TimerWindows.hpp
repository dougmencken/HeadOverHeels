// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef TimerWindows_hpp_
#define TimerWindows_hpp_

#include <WrappersAllegro.hpp>


/**
 * High precision timer for Windows systems
 */

class Timer
{

public:

        Timer();

        bool go();

        /**
         * Return seconds since the timer was started
         */
        double getValue () ;

        /**
         * Pone a cero el cronómetro
         */
        void reset();

        /**
         * Detiene el cronómetro
         */
        void stop();

        /**
         * Reinicia el cronómetro
         * @preconditions El cronómetro debe estar detenido
         */
        void restart();

protected:

        /**
         * Frecuencia
         */
        LARGE_INTEGER frequency;

        /**
         * Instante en el que se puso en marcha el cronómetro
         */
        LARGE_INTEGER startTime;

        /**
         * Instante en el que se detiene el cronómetro
         */
        LARGE_INTEGER stopTime;

        /**
         * Periodo de tiempo transcurrido entre que el cronómetro se paró y se volvió a poner en marcha
         */
        LARGE_INTEGER period;

};

#endif
