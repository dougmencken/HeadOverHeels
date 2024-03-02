// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef MapManager_hpp_
#define MapManager_hpp_

#include <string>
#include <vector>
#include <map>
#include <set>

#include "Room.hpp"


class MapManager
{

private:

        MapManager( ) : activeRoom( nilPointer ) { }

public:

        static MapManager & getInstance () ;

        virtual ~MapManager( ) ;

        void clear () ;

        virtual void beginNewGame ( const std::string & headRoom, const std::string & heelsRoom ) ;

        virtual void beginOldGameWithCharacter ( const std::string & roomFile, const std::string & characterName,
                                                 int x, int y, int z,
                                                 const std::string & direction, const std::string & entry,
                                                 bool active ) ;

        Room * changeRoom () ;

        Room * changeRoom ( const std::string & wayOfExit ) ;

        Room * rebuildRoom ( Room * room ) ;

        Room * rebuildRoom () {  return rebuildRoom( activeRoom ) ;  }

        Room * getRoomThenAddItToRoomsInPlay ( const std::string& roomFile, bool markVisited ) ;

        /**
         * Change the active room to the next room
         */
        Room * swapRoom () ;

        /**
         * Remove the active room and activate the room where the other character is.
         * Used when a character loses all its lives
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
         * @return room or nil if there’re no more characters
         */
        Room * getRoomOfInactiveCharacter () const ;

        bool isRoomInPlay ( const Room * room ) const ;

        Room * findRoomInPlayByFile ( const std::string& roomFile ) const ;

        Room * findRoomByFile ( const std::string& roomFile ) const ;

        Room * getOrBuildRoomByFile ( const std::string& roomFile ) ;

        const std::set< std::string > & listAllVisitedRooms () const {  return visitedRooms ;  }

        unsigned int howManyVisitedRooms () const {  return visitedRooms.size() ;  }

        void parseVisitedRooms ( const std::vector< std::string > & visitedRooms ) ;

        static bool buildEveryRoomAtOnce ;

private:

        static MapManager instance ;

        /**
         * The room to draw yet
         */
        Room * activeRoom ;

        std::vector < Room * > roomsInPlay ;

        std::map< std::string, ConnectedRooms* > linksBetweenRooms ;

        /**
         * Every room on the map
         */
        std::map < std::string, Room * > gameRooms ;

        /*
         * when any character visits a room, the unique name of room's file is added to this set
         */
        std::set < std::string > visitedRooms ;

        // compose the game map by an XML file
        void readMap ( const std::string & fileName ) ;

        void addRoomAsVisited( const std::string & roomFile ) {  visitedRooms.insert( roomFile ) ;  }

        // after such forgetting, no room will be counted as visited
        void forgetVisitedRooms () {  visitedRooms.clear () ;  }

} ;

#endif
