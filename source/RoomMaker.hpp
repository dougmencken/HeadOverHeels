// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef RoomMaker_hpp_
#define RoomMaker_hpp_

#include <string>

#include <tinyxml2.h>

#include "Way.hpp"
#include "FloorTile.hpp"
#include "WallPiece.hpp"
#include "GridItem.hpp"
#include "FreeItem.hpp"
#include "AvatarItem.hpp"
#include "Door.hpp"


class Room ;

/**
 * Makes a room by constructing its various parts according to the description from file
 */

class RoomMaker
{

public:

        /**
         * Construct the room by the description from file
         */
        static Room * makeRoom ( const std::string & roomFile ) ;

private:

        RoomMaker( ) { }

        static void makeFloor( Room * room, tinyxml2::XMLElement * xmlRootElement );

        static WallPiece * makeWallPiece ( tinyxml2::XMLElement * wall ) ;

        static GridItemPtr makeGridItem ( tinyxml2::XMLElement * item, Room* room ) ;

        static FreeItemPtr makeFreeItem ( tinyxml2::XMLElement * item, Room* room ) ;

        static Door * makeDoor ( tinyxml2::XMLElement * item ) ;

} ;

#endif
