// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
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
 * Cronómetro de alta precisión para sistemas Linux
 */

class HPC
{

public:

        HPC( ) ;

        virtual ~HPC( ) ;

       /**
        * Pone en marcha el cronómetro
        */
        void start () ;

       /**
        * Devuelve el tiempo transcurrido desde que el cronómetro se puso en marcha
        * @return Un valor en milisegundos
        */
        double getValue () ;

       /**
        * Pone a cero el cronómetro
        */
        void reset () ;

       /**
        * Detiene el cronómetro
        */
        void stop () ;

       /**
        * Reinicia el cronómetro
        */
        void restart () ;

protected:

       /**
        * Instante en el que se puso en marcha el cronómetro
        */
        timeval trestart ;

       /**
        * Instante en el que se detiene el cronómetro
        */
        timeval tstop ;

       /**
        * Periodo de tiempo transcurrido entre que el cronómetro se paró y se volvió a poner en
        * marcha
        */
        double period ;

       /**
        * Huso horario
        */
        struct timezone tz ;

};

#endif
