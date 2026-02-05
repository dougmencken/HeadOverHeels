// The free and open source remake of Head over Heels
//
// Copyright © 2026 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Way_hpp_
#define Way_hpp_

#include <string>


/**
 * Where an item is heading or moving,
 * or the way of entry to the room or the way of exit from the room
 */

class Way
{

public:

        explicit Way( unsigned int whichWay = Nowhere ) : way( whichWay ) { }

        explicit Way( const std::string & stringOfWay ) ;

        unsigned int getIntegerOfWay () const {  return way ;  }

        std::string toString () const ;

        Way & operator = ( unsigned int newWay )
        {
                if ( way != newWay ) way = newWay ;
                return *this ;
        }

        static const unsigned int South = 0 ;           // south, bottom right of screen
        static const unsigned int West = 1 ;            // west, bottom left of screen
        static const unsigned int North = 2 ;           // north, top left of screen
        static const unsigned int East = 3 ;            // east, top right of screen

        static const unsigned int Northeast = 4 ;
        static const unsigned int Southeast = 5 ;
        static const unsigned int Southwest = 6 ;
        static const unsigned int Northwest = 7 ;
        static const unsigned int Eastnorth = 8 ;
        static const unsigned int Eastsouth = 9 ;
        static const unsigned int Westnorth = 10 ;
        static const unsigned int Westsouth = 11 ;

        static const unsigned int Above = 22 ;          // to a room without floor
        static const unsigned int Below = 23 ;          // to a room without ceiling

        static const unsigned int ByTeleport = 33 ;     // to another room via teleport
        static const unsigned int ByTeleportToo = 34 ;  // to another room via second teleport

        static std::string exitToEntry ( const std::string & wayOfExit ) ;

private:

        static const unsigned int Nowhere = 0x500 ;

        unsigned int way ;

} ;

#endif
