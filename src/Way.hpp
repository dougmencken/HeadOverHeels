// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Way_hpp_
#define Way_hpp_

#include <string>


namespace isomot
{
        const unsigned int South = 0 ;                  // south, bottom right of screen
        const unsigned int West = 1 ;                   // west, bottom left of screen
        const unsigned int North = 2 ;                  // north, top left of screen
        const unsigned int East = 3 ;                   // east, top right of screen

        const unsigned int Northeast = 4 ;              // northeast, top of screen
        const unsigned int Southeast = 5 ;              // southeast, right side of screen
        const unsigned int Southwest = 6 ;              // southwest, bottom of screen
        const unsigned int Northwest = 7 ;              // northwest, left side of screen
        const unsigned int Eastnorth = 8 ;              // door located to the east, north
        const unsigned int Eastsouth = 9 ;              // door located to the east, south
        const unsigned int Westnorth = 10 ;             // door located to the west, north
        const unsigned int Westsouth = 11 ;             // door located to the west, south

        const unsigned int Up = 22 ;                    // to room without floor
        const unsigned int Down = 23 ;                  // to room without ceiling
        const unsigned int ByTeleport = 24 ;            // to another room via teleport
        const unsigned int ByTeleportToo = 25 ;         // to another room via second teleport

        const unsigned int NoExit = 44 ;                // player doesn’t exit from this room

        const unsigned int Restart = 51 ;               // restart room when character loses its life

        const unsigned int JustWait = 99 ;              // just wait in room

        const unsigned int Nowhere = 200 ;              // for items with only one direction


        /**
         * Way in which you move an item or entry to room or exit from room
         */

        class Way
        {

        public:

                Way( unsigned int way ) ;

                Way( const std::string& stringOfWay ) ;

                unsigned int getIntegerOfWay () const {  return way ;  }

                std::string toString () const ;

                bool operator< ( const Way& toCompare ) const {  return way < toCompare.way ;  }

        private:

                unsigned int way ;

        } ;

}

#endif
