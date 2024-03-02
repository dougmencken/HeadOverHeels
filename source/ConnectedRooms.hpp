// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ConnectedRooms_hpp_
#define ConnectedRooms_hpp_

#include <string>
#include <map>


/**
 * Other rooms connected to the same room on the game’s map
 */

class ConnectedRooms
{

public:

        ConnectedRooms( ) { }

        virtual ~ConnectedRooms( ) { }

        /**
         * When a big room with many doors is entered,
         * then the simple way of entry like "south" or "west" isn’t there
         */
        std::string clarifyTheWayOfEntryToABigRoom ( const std::string & wayOfEntry, const std::string & previousRoom ) const ;

        /**
         * Get the room connected to this one at
         */
        std::string getConnectedRoomAt ( const std::string & where ) const
        {
                std::map< std::string, std::string >::const_iterator found = connections.find( where );
                return ( found != connections.end () ) ? found->second : "" ;
        }

        void setRoomAtNorth ( const std::string & room ) {  connections[ "north" ] = room ;  }

        void setRoomAtSouth ( const std::string & room ) {  connections[ "south" ] = room ;  }

        void setRoomAtEast ( const std::string & room ) {  connections[ "east" ] = room ;  }

        void setRoomAtWest ( const std::string & room ) {  connections[ "west" ] = room ;  }

        void setRoomBelow ( const std::string & room ) {  connections[ "below" ] = room ;  }

        void setRoomAbove ( const std::string & room ) {  connections[ "above" ] = room ;  }

        void setRoomToTeleport ( const std::string & room ) {  connections[ "via teleport" ] = room ;  }

        void setRoomToTeleportToo ( const std::string & room ) {  connections[ "via second teleport" ] = room ;  }

        void setRoomAtNorthEast ( const std::string & room ) {  connections[ "northeast" ] = room ;  }

        void setRoomAtNorthWest ( const std::string & room ) {  connections[ "northwest" ] = room ;  }

        void setRoomAtSouthEast ( const std::string & room ) {  connections[ "southeast" ] = room ;  }

        void setRoomAtSouthWest ( const std::string & room ) {  connections[ "southwest" ] = room ;  }

        void setRoomAtEastNorth ( const std::string & room ) {  connections[ "eastnorth" ] = room ;  }

        void setRoomAtEastSouth ( const std::string & room ) {  connections[ "eastsouth" ] = room ;  }

        void setRoomAtWestNorth ( const std::string & room ) {  connections[ "westnorth" ] = room ;  }

        void setRoomAtWestSouth ( const std::string & room ) {  connections[ "westsouth" ] = room ;  }

private:

        std::map < std::string /* where */, std::string /* file of room */ > connections ;

} ;

#endif
