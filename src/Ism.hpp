// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Ism_hpp_
#define Ism_hpp_

#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include <memory>
#include <string>
#include <sstream>

#include "util.hpp"
#include "pointers.hpp"

#ifndef __WIN32
    #include <time.h>
#endif


/**
 * Pause subprocess for a given period of time in milliseconds
 */
void milliSleep ( unsigned long miliseconds );

#ifndef __WIN32

/**
 * Pause subprocess for a given period of time in microseconds
 */
void microSleep ( unsigned long microseconds );

/**
 * Pause subprocess for a given period of time in nanoseconds.
 * But implementation may sleep longer than given period
 */
void nanoSleep ( unsigned long nanoseconds );

#endif

namespace iso
{

        unsigned int ScreenWidth () ;

        void setScreenWidth ( unsigned int w ) ;

        unsigned int ScreenHeight () ;

        void setScreenHeight ( unsigned int h ) ;

        std::string makeRandomString ( const size_t length ) ;

        std::string nameFromPath ( std::string const& path ) ;

        const char * pathToFile ( const std::string& folder, const std::string& file = std::string() ) ;

        void setPathToGame ( const char * pathToGame ) ;

        /**
         * Devuelve la ruta absoluta a la carpeta personal del usuario
         */
        std::string homePath () ;

        /**
         * Devuelve la ruta absoluta a la carpeta donde residen los datos de la aplicación
         */
        std::string sharePath () ;

        /**
         * Object-function to finalize elements of Standard Template Library container
         */
        struct DeleteIt
        {
                template < typename T >
                void operator() ( const T* ptr ) const
                {
                        delete ptr ;
                }
        } ;

        const int Top = -1 ;

        /**
         * Height in isometric units of layer
         * Item in the grid at height n is n * LayerHeight units
         */
        const int LayerHeight = 24 ;

        /**
         * Maximum number of layers in room
         * In isometric units maximum height of room is LayerHeight * MaxLayers
         */
        const int MaxLayers = 10 ;

}

#endif
