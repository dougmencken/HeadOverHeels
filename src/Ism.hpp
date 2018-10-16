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

#include <sys/stat.h>
#include <sys/types.h>

#include <memory>
#include <string>
#include <sstream>

#ifndef __WIN32
    #include <time.h>
#endif

#if __cplusplus >= 201103L /* when complier supports c++11 */
    #define __Cxx11__
    #define nilPointer nullptr
    #define smartptr std::unique_ptr
#else
    #define nilPointer NULL
    #define smartptr std::auto_ptr
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

        inline bool folderExists( const std::string& path )
        {
                struct stat info;

                if ( stat( path.c_str (), &info ) < 0 )
                        return false;
                else if ( ( info.st_mode & S_IFDIR ) != 0 )
                        return true;

                return false;
        }

        unsigned int ScreenWidth () ;

        void setScreenWidth ( unsigned int w ) ;

        unsigned int ScreenHeight () ;

        void setScreenHeight ( unsigned int h ) ;

        std::string makeRandomString ( const size_t length ) ;

        std::string toStringWithOrdinalSuffix ( unsigned int number ) ;

        inline static std::string numberToString ( int number )
        {
                return static_cast< std::ostringstream * >( &( std::ostringstream() << std::dec << number ) )->str () ;
        }

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
                ( void ) howDeep ;
                fprintf( out, "no backtrace for non-debug build or when there’s no execinfo.h\n" );
#endif
        }

#ifdef __WIN32
        struct IsPathSeparator
        {
                bool operator() ( char c ) const {  return c == '\\' || c == '/' ;  }
        };
#else
        struct IsPathSeparator
        {
                bool operator() ( char c ) const {  return c == '/' ;  }
        };
#endif

        std::string nameFromPath ( std::string const& path ) ;

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
         * Object-function to finalize elements of Standard Template Library container
         */
        struct DeleteIt
        {
                template < typename T >
                void operator() ( const T* ptr ) const
                {
                        delete ptr;
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
