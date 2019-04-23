// The free and open source remake of Head over Heels
//
// Copyright © 2019 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef RoomConnections_hpp_
#define RoomConnections_hpp_

#include <string>

#include "Ism.hpp"
#include "Way.hpp"


namespace iso
{

/**
 * Connections of room with another rooms of map
 */

class RoomConnections
{

public:

        RoomConnections( ) { }

        virtual ~RoomConnections( ) { }

        /**
         * Find room connected to this one
         */
        std::string findConnectedRoom ( const std::string & wayOfExit, Way * wayOfEntry ) const ;

        /**
         * See if entrance to this room is okay or fix it if it’s not
         *
         * When triple or quadruple room is entered from single or double room
         * then entrance isn’t okay because map returns simple way like south or west
         * but doors of triple and quadruple rooms are out of such ways
         */
        void adjustEntry ( Way * wayOfEntry, const std::string & previousRoom ) const ;

private:

        /**
         * File for room to the north of this room, empty string if no room there
         */
        std::string north ;

        /**
         * File for room to the south of this room, empty string if no room there
         */
        std::string south ;

        /**
         * File for room to the east of this room, empty string if no room there
         */
        std::string east ;

        /**
         * File for room to the west of this room, empty string if no room there
         */
        std::string west ;

        /**
         * File for room below this room, empty string if no room there
         */
        std::string floor ;

        /**
         * File for room above this room, empty string if no room there
         */
        std::string roof ;

        /**
         * File for room to teleport from this room, empty string when no room to teleport to
         */
        std::string teleport ;

        std::string teleport2 ;

        /**
         * File for room located at north-east for triple or quadruple rooms, or empty string
         */
        std::string northeast ;

        /**
         * File for room located at north-west for triple or quadruple rooms, or empty string
         */
        std::string northwest ;

        /**
         * File for room located at south-east for triple or quadruple rooms, or empty string
         */
        std::string southeast ;

        /**
         * File for room located at south-west for triple or quadruple rooms, or empty string
         */
        std::string southwest ;

        /**
         * File for room located at east-north for triple or quadruple rooms, or empty string
         */
        std::string eastnorth ;

        /**
         * File for room located at east-south for triple or quadruple rooms, or empty string
         */
        std::string eastsouth ;

        /**
         * File for room located at west-north for triple or quadruple rooms, or empty string
         */
        std::string westnorth ;

        /**
         * File for room located at west-south for triple or quadruple rooms, or empty string
         */
        std::string westsouth ;

public:

        void setNorth ( const std::string& room ) {  this->north = room ;  }

        void setSouth ( const std::string& room ) {  this->south = room ;  }

        void setEast ( const std::string& room ) {  this->east = room ;  }

        void setWest ( const std::string& room ) {  this->west = room ;  }

        void setFloor ( const std::string& room ) {  this->floor = room ;  }

        void setRoof ( const std::string& room ) {  this->roof = room ;  }

        void setTeleport ( const std::string& room ) {  this->teleport = room ;  }

        void setTeleportToo ( const std::string& room ) {  this->teleport2 = room ;  }

        void setNorthEast ( const std::string& room ) {  this->northeast = room ;  }

        void setNorthWest ( const std::string& room ) {  this->northwest = room ;  }

        void setSouthEast ( const std::string& room ) {  this->southeast = room ;  }

        void setSouthWest ( const std::string& room ) {  this->southwest = room ;  }

        void setEastNorth ( const std::string& room ) {  this->eastnorth = room ;  }

        void setEastSouth ( const std::string& room ) {  this->eastsouth = room ;  }

        void setWestNorth ( const std::string& room ) {  this->westnorth = room ;  }

        void setWestSouth ( const std::string& room ) {  this->westsouth = room ;  }

};

}

#endif
