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
#include <sstream>

#include <WrappersAllegro.hpp>

#include <tinyxml2.h>

#include "Ism.hpp"
#include "Way.hpp"


namespace isomot
{

class ItemData ;
class Room ;
class FloorTile ;
class Wall ;
class GridItem ;
class FreeItem ;
class PlayerItem ;
class Door ;

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
        static PlayerItem * createPlayerInRoom ( Room* room,
                                                 const std::string& nameOfPlayer,
                                                 bool justEntered,
                                                 int x, int y, int z,
                                                 const Way& orientation, const Way& wayOfEntry = JustWait );

        static int getXCenterOfRoom ( ItemData* data, Room* theRoom ) ;

        static int getYCenterOfRoom ( ItemData* data, Room* theRoom ) ;

private:

        RoomBuilder( ) { }

        static FloorTile* buildFloorTile ( tinyxml2::XMLElement * tile, const char* gfxPrefix ) ;

        static Wall* buildWall ( tinyxml2::XMLElement * wall, const char* gfxPrefix ) ;

        static GridItem* buildGridItem ( tinyxml2::XMLElement * item, Room* room ) ;

        static FreeItem* buildFreeItem ( tinyxml2::XMLElement * item, Room* room ) ;

        static Door* buildDoor ( tinyxml2::XMLElement * item ) ;

};

}

#endif
