// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef RoomBuilder_hpp_
#define RoomBuilder_hpp_

#include <string>
#include <iostream>

#include <tinyxml2.h>

#include "Ism.hpp"
#include "Way.hpp"
#include "FloorTile.hpp"
#include "Wall.hpp"
#include "GridItem.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "Door.hpp"


namespace iso
{

class Room ;

/**
 * Creates a room by constructing different parts of it via data from file
 */

class RoomBuilder
{

public:

        /**
         * Construct room by data from file
         */
        static Room * buildRoom ( const std::string& roomFile ) ;

        /**
         * Create player in given room
         * @param room Room where to create player
         * @param justEntered Make copy of player used to recreate it when rebuilding room or not
         */
        static PlayerItemPtr createPlayerInRoom ( Room* room,
                                                  const std::string& nameOfPlayer,
                                                  bool justEntered,
                                                  int x, int y, int z,
                                                  const Way& orientation, const Way& wayOfEntry = Way::JustWait );

private:

        RoomBuilder( ) { }

        static Wall * buildWall ( tinyxml2::XMLElement * wall, const std::string& gfxPrefix ) ;

        static GridItemPtr buildGridItem ( tinyxml2::XMLElement * item, Room* room ) ;

        static FreeItemPtr buildFreeItem ( tinyxml2::XMLElement * item, Room* room ) ;

        static Door * buildDoor ( tinyxml2::XMLElement * item ) ;

};

}

#endif
