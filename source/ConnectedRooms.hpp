// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
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
                std::map< std::string, std::string >::const_iterator found = this->connections.find( where );
                return ( found != this->connections.end () ) ? found->second : "" ;
        }

        void setConnectedRoomAt ( const std::string & where, const std::string & room )
        {
                std::string at( where );
                     if ( where ==  "teleport" ) at = "via teleport" ;
                else if ( where == "teleport2" ) at = "via second teleport" ;

                this->connections[ at ] = room ;
        }

private:

        std::map < std::string /* where */, std::string /* file of room */ > connections ;

} ;

#endif
