// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef RoomConnections_hpp_
#define RoomConnections_hpp_

#include <string>
#include <map>

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
         * See if entrance to this room is okay or fix it when it’s not
         *
         * When triple or quadruple room is entered from single or double room
         * then entrance isn’t okay because map returns simple way like south or west
         */
        void adjustEntry ( Way * wayOfEntry, const std::string & previousRoom ) const ;

        std::string getConnectedRoomAt ( const std::string & where ) const
        {
                std::map< std::string, std::string >::const_iterator found = connections.find( where );
                return ( found != connections.end () ) ? found->second : "" ;
        }

        /**
         * File for room to the north of this room, empty string if no room there
         */
        std::string getNorth () const {  return getConnectedRoomAt( "north" ) ;  }

        void setNorth ( const std::string & room ) {  connections[ "north" ] = room ;  }

        /**
         * File for room to the south of this room, empty string if no room there
         */
        std::string getSouth () const {  return getConnectedRoomAt( "south" ) ;  }

        void setSouth ( const std::string & room ) {  connections[ "south" ] = room ;  }

        /**
         * File for room to the east of this room, empty string if no room there
         */
        std::string getEast () const {  return getConnectedRoomAt( "east" ) ;  }

        void setEast ( const std::string & room ) {  connections[ "east" ] = room ;  }

        /**
         * File for room to the west of this room, empty string if no room there
         */
        std::string getWest () const {  return getConnectedRoomAt( "west" ) ;  }

        void setWest ( const std::string & room ) {  connections[ "west" ] = room ;  }

        /**
         * File for room below this room, empty string if no room there
         */
        std::string getFloor () const {  return getConnectedRoomAt( "below" ) ;  }

        void setFloor ( const std::string & room ) {  connections[ "below" ] = room ;  }

        /**
         * File for room above this room, empty string if no room there
         */
        std::string getRoof () const {  return getConnectedRoomAt( "above" ) ;  }

        void setRoof ( const std::string & room ) {  connections[ "above" ] = room ;  }

        /**
         * File for room connected via teleport to this room, empty string when no teleport
         */
        std::string getTeleport () const {  return getConnectedRoomAt( "via teleport" ) ;  }

        void setTeleport ( const std::string & room ) {  connections[ "via teleport" ] = room ;  }

        std::string getTeleportToo () const {  return getConnectedRoomAt( "via second teleport" ) ;  }

        void setTeleportToo ( const std::string & room ) {  connections[ "via second teleport" ] = room ;  }

        /**
         * File for room located at north-east for triple or quadruple rooms, or empty string
         */
        std::string getNorthEast () const {  return getConnectedRoomAt( "northeast" ) ;  }

        void setNorthEast ( const std::string & room ) {  connections[ "northeast" ] = room ;  }

        /**
         * File for room located at north-west for triple or quadruple rooms, or empty string
         */
        std::string getNorthWest () const {  return getConnectedRoomAt( "northwest" ) ;  }

        void setNorthWest ( const std::string & room ) {  connections[ "northwest" ] = room ;  }

        /**
         * File for room located at south-east for triple or quadruple rooms, or empty string
         */
        std::string getSouthEast () const {  return getConnectedRoomAt( "southeast" ) ;  }

        void setSouthEast ( const std::string & room ) {  connections[ "southeast" ] = room ;  }

        /**
         * File for room located at south-west for triple or quadruple rooms, or empty string
         */
        std::string getSouthWest () const {  return getConnectedRoomAt( "southwest" ) ;  }

        void setSouthWest ( const std::string & room ) {  connections[ "southwest" ] = room ;  }

        /**
         * File for room located at east-north for triple or quadruple rooms, or empty string
         */
        std::string getEastNorth () const {  return getConnectedRoomAt( "eastnorth" ) ;  }

        void setEastNorth ( const std::string & room ) {  connections[ "eastnorth" ] = room ;  }

        /**
         * File for room located at east-south for triple or quadruple rooms, or empty string
         */
        std::string getEastSouth () const {  return getConnectedRoomAt( "eastsouth" ) ;  }

        void setEastSouth ( const std::string & room ) {  connections[ "eastsouth" ] = room ;  }

        /**
         * File for room located at west-north for triple or quadruple rooms, or empty string
         */
        std::string getWestNorth () const {  return getConnectedRoomAt( "westnorth" ) ;  }

        void setWestNorth ( const std::string & room ) {  connections[ "westnorth" ] = room ;  }

        /**
         * File for room located at west-south for triple or quadruple rooms, or empty string
         */
        std::string getWestSouth () const {  return getConnectedRoomAt( "westsouth" ) ;  }

        void setWestSouth ( const std::string & room ) {  connections[ "westsouth" ] = room ;  }

private:

        std::map < std::string /* where */, std::string /* file of room */ > connections ;

};

}

#endif
