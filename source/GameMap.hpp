// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef GameMap_hpp_
#define GameMap_hpp_

#include <string>
#include <vector>
#include <map>
#include <set>

#include "Room.hpp"


class GameMap
{

private:

        GameMap( ) : activeRoom( nilPointer ) { }

public:

        static GameMap & getInstance () ;

        virtual ~GameMap( ) ;

        void clear () ;

        virtual void beginNewGame ( const std::string & headRoom, const std::string & heelsRoom ) ;

        virtual void beginOldGameWithCharacter ( const std::string & roomFile, const std::string & characterName,
                                                 int x, int y, int z, const std::string & heading,
                                                 bool activeCharacter ) ;

        Room * changeRoom () ;

        Room * changeRoom ( const std::string & wayOfExit ) ;

        void rebuildRoom ( Room * room ) ;

        void rebuildRoom () {  rebuildRoom( this->activeRoom ) ;  }

        void rebuildRoom ( const std::string & roomFile ) {  rebuildRoom( findRoomByFile( roomFile ) ) ;  }

        Room * getRoomThenAddItToRoomsInPlay ( const std::string& roomFile, bool markVisited ) ;

        /**
         * Change the active room to the next room
         */
        Room * swapRoom () ;

        /**
         * Remove the current room and activate the room where the other character is.
         * Used when a character loses all lives
         */
        void noLivesSwap () ;

        void addRoomInPlay ( Room * whichRoom ) ;

        void removeRoomInPlay ( Room * whichRoom ) ;

        void binRoomsInPlay () ;

        /**
         * The room being drawn
         */
        Room * getActiveRoom () const {  return this->activeRoom ;  }

        void setActiveRoom ( Room * newRoom )
        {
                this->activeRoom = newRoom ;
                if ( this->activeRoom != nilPointer ) this->activeRoom->activate () ;
        }

        /**
         * @return the room in which the inactive character is,
         *         or nil if there’re no more characters or the other character
         *         is in the same room
         */
        Room * getRoomOfInactiveCharacter () const ;

        bool isRoomInPlay ( const Room * room ) const ;

        Room * findRoomInPlayByFile ( const std::string& roomFile ) const ;

        Room * findRoomByFile ( const std::string& roomFile ) const ;

        Room * getOrBuildRoomByFile ( const std::string& roomFile ) ;

        /**
         * useful after rebuilding a room, also deletes the previous room
         */
        void replaceRoomForFile( const std::string & roomFile, Room * room );

        const std::set< std::string > & listAllVisitedRooms () const {  return visitedRooms ;  }

        unsigned int howManyVisitedRooms () const {  return visitedRooms.size() ;  }

        void parseVisitedRooms ( const std::vector< std::string > & visitedRooms ) ;

        void getAllRoomFiles ( std::vector< std::string > & whereToCollect ) ;

        static void setBuildEveryRoomAtOnce ( bool allAtOnce ) {  buildEveryRoomAtOnce = allAtOnce ;  }

private:

        static GameMap instance ;

        static bool buildEveryRoomAtOnce ;

        // the room being drawn
        Room * activeRoom ;

        std::vector < Room * > roomsInPlay ;

        std::map < std::string, ConnectedRooms* > linksBetweenRooms ;

        /**
         * Every room on the map
         */
        std::map < std::string, Room * > gameRooms ;

        /*
         * When any character visits a room, the room’s file name is added to this set
         */
        std::set < std::string > visitedRooms ;

        /**
         * Read the game map from an XML file
         */
        void readMap ( const std::string & fileName ) ;

        void addRoomAsVisited( const std::string & roomFile ) {  visitedRooms.insert( roomFile ) ;  }

        // after such forgetting, no room will be counted as visited
        void forgetVisitedRooms () {  visitedRooms.clear () ;  }

} ;

#endif
