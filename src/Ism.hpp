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

#include <string>
#include <utility>
#include <functional>
#include <fstream>
#include <cstdlib>
#include <cassert>

#include <allegro.h>

#ifdef __WIN32
    #include <winalleg.h>
#else
    #include <time.h>
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

#if __cplusplus >= 201103L /* when complier supports c++11 */
    #define nilPointer nullptr
    #define __Cxx11__
#else
    #define nilPointer NULL
#endif

#if defined( DEBUG ) && defined( HAVE_EXECINFO_H ) && HAVE_EXECINFO_H
    #include <execinfo.h>
    #include <cxxabi.h>
#endif

#if defined ( __WIN32 ) && ! defined ( __CYGWIN__ )
    const std::string pathSeparator = "\\" ;
#else
    const std::string pathSeparator = "/" ;
#endif

#ifdef ALLEGRO_BIG_ENDIAN
    # define IS_BIG_ENDIAN 1
#else
    # define IS_BIG_ENDIAN 0
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

namespace isomot
{

        std::string makeRandomString( const size_t length ) ;

        /** print backtrace of caller */
        inline void printBacktrace ( FILE * out = stdout, unsigned int howDeep = 80 )
        {
#if defined( DEBUG ) && defined( HAVE_EXECINFO_H ) && HAVE_EXECINFO_H
                fprintf( out, "backtrace\n" );

                // storage for data of backtrace addresses
                void* addrlist[ howDeep + 1 ];

                // get addresses of backtrace
                size_t addrlen = backtrace( addrlist, sizeof( addrlist ) / sizeof( void * ) );

                if ( addrlen == 0 ) {
                        fprintf( out, "    < empty, possibly broken >\n" );
                        return;
                }

                // convert addresses into strings
                char** listOfSymbols = backtrace_symbols( addrlist, addrlen );

                for ( size_t i = 0; i < addrlen; i++ )
                {
                        fprintf( out, "    %s\n", listOfSymbols[ i ] );
                }

                free( listOfSymbols );
#else
                fprintf( out, "no backtrace for non-debug build or when there’s no execinfo.h\n" );
#endif
        }

        typedef std::pair< int, int > JumpMotion;

        const char * pathToFile ( const std::string& in ) ;

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
         * Object-function to finalize objects of Standard Template Library container
         */
        struct DeleteObject
        {
                template < typename T >
                void operator() ( const T* ptr ) const
                {
                        delete ptr;
                }
        } ;

        enum ActivityOfItem
        {
                Wait,                           /* El elemento está quieto */
                Push,                           /* El elemento ha sido empujado por otro */
                Move,                           /* El elemento se mueve. Estado genérico usado por el gestor de sonido */
                MoveNorth = 64,                 /* El elemento se mueve al norte */
                MoveSouth,                      /* El elemento se mueve al sur */
                MoveEast,                       /* El elemento se mueve al este */
                MoveWest,                       /* El elemento se mueve al oeste */
                MoveNortheast,                  /* El elemento se mueve al nordeste */
                MoveSoutheast,                  /* El elemento se mueve al sudeste */
                MoveSouthwest,                  /* El elemento se mueve al sudoeste */
                MoveNorthwest,                  /* El elemento se mueve al noroeste */
                MoveUp,                         /* El elemento se mueve arriba */
                MoveDown,                       /* El elemento se mueve abajo */
                Blink,                          /* El elemento parpadea */
                Jump,                           /* El elemento salta */
                RegularJump,                    /* El elemento salta de modo normal */
                HighJump,                       /* El elemento salta de modo especial, a mayor altura y desplazamiento */
                Fall,                           /* El elemento cae */
                Glide,                          /* El elemento planea */
                TakeItem,                       /* El elemento coge otro elemento */
                TakenItem,                      /* El elemento ha cogido otro elemento */
                DropItem,                       /* El elemento suelta a otro elemento previamente cogido */
                TakeAndJump,                    /* El elemento coge otro elemento y luego salta */
                DropAndJump,                    /* El elemento deja otro elemento y luego salta */
                DisplaceNorth,                  /* El elemento es desplazado al norte por otro elemento */
                DisplaceSouth,                  /* El elemento es desplazado al sur por otro elemento */
                DisplaceEast,                   /* El elemento es desplazado al este por otro elemento */
                DisplaceWest,                   /* El elemento es desplazado al oeste por otro elemento */
                DisplaceNortheast,              /* El elemento es desplazado al noreste por otro elemento */
                DisplaceSoutheast,              /* El elemento es desplazado al sudeste por otro elemento */
                DisplaceSouthwest,              /* El elemento es desplazado al sudoeste por otro elemento */
                DisplaceNorthwest,              /* El elemento es desplazado al noroeste por otro elemento */
                DisplaceUp,                     /* El elemento es desplazado hacia arriba por otro elemento */
                DisplaceDown,                   /* El elemento es desplazado hacia abajo por otro elemento */
                BeginWayOutTeletransport,
                WayOutTeletransport,
                BeginWayInTeletransport,
                WayInTeletransport,
                AutoMove,                       /* El elemento se mueve automáticamente. Estado genérico usado por el gestor de sonido */
                AutoMoveNorth,                  /* El elemento se mueve automáticamente al norte */
                AutoMoveSouth,                  /* El elemento se mueve automáticamente al sur */
                AutoMoveEast,                   /* El elemento se mueve automáticamente al este */
                AutoMoveWest,                   /* El elemento se mueve automáticamente al oeste */
                ForceDisplace,
                ForceDisplaceNorth,             /* El elemento se desplaza automáticamente al norte */
                ForceDisplaceSouth,             /* El elemento se desplaza automáticamente al sur */
                ForceDisplaceEast,              /* El elemento se desplaza automáticamente al este */
                ForceDisplaceWest,              /* El elemento se desplaza automáticamente al oeste */
                CancelDisplaceNorth,            /* El elemento deja de desplazarse automáticamente al norte */
                CancelDisplaceSouth,            /* El elemento deja de desplazarse automáticamente al sur */
                CancelDisplaceEast,             /* El elemento deja de desplazarse automáticamente al este */
                CancelDisplaceWest,             /* El elemento deja de desplazarse automáticamente al oeste */
                Freeze,                         /* El elemento está detenido por la acción de un rosquillazo o por la activación de un interruptor */
                WakeUp,                         /* El elemento se vuelve a activar por la desactivación del interruptor */
                StopTop,                        /* El elemento ha alcanzado su altura máxima. Aplicado a elementos de movimiento vertical */
                StopBottom,                     /* El elemento ha alcanzado su altura mínima. Aplicado a elementos de movimiento vertical */
                MeetMortalItem,
                Vanish,
                FireDoughnut,
                Rebound,                        /* El elemento rebota */
                SwitchIt,                       /* El elemento (un interruptor) cambia de estado */
                Collision,                      /* El elemento choca con algo */
                IsActive,
                Mistake                         /* Estado imposible, se utiliza en el gestor de sonido */
        } ;

        const unsigned int ScreenWidth = 640 ;

        const unsigned int ScreenHeight = 480 ;

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
