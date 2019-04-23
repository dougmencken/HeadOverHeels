// The free and open source remake of Head over Heels
//
// Copyright © 2019 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef MapManager_hpp_
#define MapManager_hpp_

#include <string>
#include <vector>
#include <map>

#include "Room.hpp"


namespace iso
{

class MapManager
{

public:

        MapManager( ) : activeRoom( nilPointer ) { }

        virtual ~MapManager( ) ;

        void clear () ;

        virtual void beginNewGame ( const std::string& headRoom, const std::string& heelsRoom ) ;

        virtual void beginOldGameWithCharacter ( const std::string& roomFile, const std::string& character,
                                                 int x, int y, int z, const std::string& direction, const std::string& entry,
                                                 bool active ) ;

        Room * changeRoom () ;

        Room * changeRoom ( const std::string & wayOfExit ) ;

        Room * rebuildRoom ( Room * room ) ;

        Room * rebuildRoom () {  return rebuildRoom( activeRoom ) ;  }

        Room * getRoomThenAddItToRoomsInPlay ( const std::string& roomFile, bool markVisited ) ;

        /**
         * Change active room to next room
         */
        Room * swapRoom () ;

        /**
         * Remove active room and activate room where the other player is. Used when player
         * loses all its lives
         */
        Room * noLivesSwap () ;

        void addRoomInPlay ( Room * whichRoom ) ;

        void removeRoomInPlay ( Room * whichRoom ) ;

        void binRoomsInPlay () ;

        /**
         * The room to draw yet
         */
        Room * getActiveRoom () const {  return activeRoom ;  }

        void setActiveRoom ( Room * newRoom ) {  activeRoom = newRoom ;  }

        /**
         * @return room or nil if there’re no more players
         */
        Room * getRoomOfInactivePlayer () const ;

        bool isRoomInPlay ( const Room * room ) const ;

        Room * findRoomInPlayByFile ( const std::string& roomFile ) const ;

        Room * findRoomByFile ( const std::string& roomFile ) const ;

        Room * getOrBuildRoomByFile ( const std::string& roomFile ) ;

        void parseVisitedRooms ( const std::vector< std::string >& visitedRooms ) ;

        void fillVisitedRooms ( std::vector< std::string >& visitedSequence ) ;

        unsigned int countVisitedRooms () ;

        void resetVisitedRooms () ;

        static bool buildEveryRoomAtOnce ;

private:

        /**
         * Compose map from XML file
         */
        void readMap ( const std::string& fileName ) ;

        /**
         * The room to draw yet
         */
        Room * activeRoom ;

        std::vector < Room * > roomsInPlay ;

        std::map< std::string, RoomConnections* > linksBetweenRooms ;

        /**
         * Every room on map
         */
        std::map < std::string, Room * > gameRooms ;

};

}

#endif
