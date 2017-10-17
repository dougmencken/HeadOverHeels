// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
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
#include <allegro.h>
#include "MapRoomData.hpp"
#include "csxml/MapXML.hpp"
#include "csxml/SaveGameXML.hpp"

# include "Room.hpp"
# include "ItemDataManager.hpp"


namespace isomot
{

class Isomot;
class Room;


class MapManager
{

public:

        /**
         * Constructor
         * @param isomot El motor isométrico
         * @param fileName Nombre del archivo XML que contiene los datos del mapa
         */
        MapManager( Isomot * isomot, const std::string& fileName ) ;

        virtual ~MapManager( ) ;

        /**
         * Compose data for map from XML file
         */
        void loadMap () ;

        virtual void beginNewGame ( const std::string& firstRoomFileName, const std::string& secondRoomFileName ) ;

        void beginOldGameWithCharacter ( const sgxml::player& data ) ;

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
         * @return La sala creada ó 0 si no se pudo construir
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

        void readVisitedSequence ( sgxml::exploredRooms::visited_sequence& visitedSequence ) ;

        void storeVisitedSequence ( sgxml::exploredRooms::visited_sequence& visitedSequence ) ;

        /**
         * Cuenta el número de salas visitadas por los jugadores
         */
        unsigned int countVisitedRooms () ;

        void resetVisitedRooms () ;

        /**
         * @param room name of file for room
         */
        MapRoomData * findRoomData ( const std::string& room ) ;

        /**
         * Look for room in list of created rooms
         * @param room name of file for room
         * @return found room or 0 if it’s absent
         */
        Room * findRoom ( const std::string& room ) ;

protected:

        Isomot* isomot ;

        std::vector< Room * > rooms ;

        /**
         * The room to draw yet
         */
        Room * activeRoom ;

        /**
         * Name of XML file with map data
         */
        std::string fileName ;

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
         * @return room or 0 if there’re no more players
         */
        Room * getRoomOfInactivePlayer () ;

};


class EqualMapRoomData : public std::binary_function< MapRoomData *, std::string, bool >
{

public:
        bool operator()( const MapRoomData * mapData, const std::string& room ) const;

};

class EqualRoom : public std::binary_function< Room *, std::string, bool >
{

public:
        bool operator()( const Room * room, const std::string& nameOfRoom ) const;

};

}

#endif
