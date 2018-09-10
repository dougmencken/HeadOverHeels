// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef MapManager_hpp_
#define MapManager_hpp_

#include <string>
#include <list>
#include <iostream>

#include <WrappersAllegro.hpp>

#include "MapRoomData.hpp"
#include "Room.hpp"
#include "ItemDataManager.hpp"


namespace isomot
{

class Isomot;
class Room;


class MapManager
{

public:

        MapManager( Isomot * isomot ) ;

        virtual ~MapManager( ) ;

        /**
         * Compose data for map from XML file
         */
        void loadMap ( const std::string& fileName ) ;

        virtual void beginNewGame ( const std::string& headRoom, const std::string& heelsRoom ) ;

        void beginOldGameWithCharacter ( const std::string& roomFile, const std::string& character,
                                         int x, int y, int z, const Way& direction, const std::string& entry, bool active ) ;

        void binEveryRoom () ;

        /**
         * Change active room by way of exit chosen
         */
        Room * changeRoom ( const Way& wayOfExit ) ;

        /**
         * Recreate active room
         */
        Room * rebuildRoom () ;

        /**
         * Create room by data from file
         */
        Room * createRoom ( const std::string& fileName ) ;

        Room * createRoomThenAddItToListOfRooms ( const std::string& fileName, bool markVisited ) ;

        /**
         * Change active room to next room
         */
        Room * swapRoom () ;

        /**
         * Remove active room and activate room where the other player is. Used when player
         * loses all its lives
         */
        Room * removeRoomAndSwap () ;

        void removeRoom ( Room* whichRoom ) ;

        void parseVisitedRooms ( const std::vector< std::string >& visitedRooms ) ;

        void fillVisitedRooms ( std::vector< std::string >& visitedSequence ) ;

        unsigned int countVisitedRooms () ;

        void resetVisitedRooms () ;

        MapRoomData * findRoomData ( const std::string& roomFile ) const ;

        MapRoomData * findRoomData ( const Room * room ) const {  return findRoomData( room->getNameOfFileWithDataAboutRoom() ) ;  }

        Room * findRoomByFile ( const std::string& roomFile ) const ;

protected:

        Isomot* isomot ;

        std::list< Room * > rooms ;

        /**
         * The room to draw yet
         */
        Room * activeRoom ;

        /**
         * Data of every room on map
         */
        std::list < MapRoomData * > theMap ;

public:

        /**
         * The room to draw yet
         */
        Room * getActiveRoom () const {  return activeRoom ;  }

        void setActiveRoom ( Room * newRoom ) {  activeRoom = newRoom ;  }

        /**
         * @return room or nil if there’re no more players
         */
        Room * getRoomOfInactivePlayer () const ;

};

}

#endif
