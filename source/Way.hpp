// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Way_hpp_
#define Way_hpp_

#include <string>


namespace iso
{

/**
 * The way in which you move an item or the entry to a room or the exit from a room
 */

class Way
{

public:

        explicit Way( unsigned int way = Nowhere ) : way( way ) { }

        explicit Way( const std::string & stringOfWay ) ;

        unsigned int getIntegerOfWay () const {  return way ;  }

        std::string toString () const ;

        bool operator < ( const Way & toCompare ) const {  return way < toCompare.way ;  }

        Way & operator = ( unsigned int newWay )
        {
                if ( way != newWay ) way = newWay ;
                return *this ;
        }

        static const unsigned int South = 0 ;           // south, bottom right of screen
        static const unsigned int West = 1 ;            // west, bottom left of screen
        static const unsigned int North = 2 ;           // north, top left of screen
        static const unsigned int East = 3 ;            // east, top right of screen

        static const unsigned int Northeast = 4 ;       // northeast, top of screen
        static const unsigned int Southeast = 5 ;       // southeast, right side of screen
        static const unsigned int Southwest = 6 ;       // southwest, bottom of screen
        static const unsigned int Northwest = 7 ;       // northwest, left side of screen
        static const unsigned int Eastnorth = 8 ;       // door located to the east, north
        static const unsigned int Eastsouth = 9 ;       // door located to the east, south
        static const unsigned int Westnorth = 10 ;      // door located to the west, north
        static const unsigned int Westsouth = 11 ;      // door located to the west, south

        static const unsigned int Above = 22 ;          // to room without floor
        static const unsigned int Below = 23 ;          // to room without ceiling

        static const unsigned int ByTeleport = 33 ;     // to another room via teleport
        static const unsigned int ByTeleportToo = 34 ;  // to another room via second teleport

        static const unsigned int DidNotQuit = 44 ;     // the character has not yet quit the room

        static const unsigned int RestartRoom = 55 ;    // restart the room, for example when the character loses one life

        static const unsigned int JustWait = 99 ;       // just wait in room

        static const unsigned int Nowhere = 200 ;       // for items with only one direction

private:

        unsigned int way ;

} ;

}

#endif
