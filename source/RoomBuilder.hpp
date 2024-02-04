// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef RoomBuilder_hpp_
#define RoomBuilder_hpp_

#include <string>
#include <iostream>

#include <tinyxml2.h>

#include "Way.hpp"
#include "FloorTile.hpp"
#include "Wall.hpp"
#include "GridItem.hpp"
#include "FreeItem.hpp"
#include "AvatarItem.hpp"
#include "Door.hpp"


class Room ;

/**
 * Creates a room by constructing different parts of it via data from file
 */

class RoomBuilder
{

public:

        /**
         * Construct the room by the data from file
         */
        static Room * buildRoom ( const std::string & roomFile ) ;

        /**
         * Create a character in a specified room
         * @param justEntered Make the copy of character to recreate it when rebuilding the room or don't
         */
        static AvatarItemPtr createCharacterInRoom ( Room * room,
                                                     const std::string & nameOfCharacter,
                                                     bool justEntered,
                                                     int x, int y, int z,
                                                     const std::string & orientation,
                                                     const std::string & wayOfEntry = "just wait" );

private:

        RoomBuilder( ) { }

        static Wall * buildWall ( tinyxml2::XMLElement * wall ) ;

        static GridItemPtr buildGridItem ( tinyxml2::XMLElement * item, Room* room ) ;

        static FreeItemPtr buildFreeItem ( tinyxml2::XMLElement * item, Room* room ) ;

        static Door * buildDoor ( tinyxml2::XMLElement * item ) ;

} ;

#endif
