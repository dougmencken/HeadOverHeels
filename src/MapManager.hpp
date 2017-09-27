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
#include <algorithm>
#include <iostream>
#include <functional>
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

        /**
         * Limpia los datos almacenados relacionados con la partida en curso
         */
        void reset () ;

        /**
         * Cambia la sala activa en función de la salida tomada por el jugador
         * @param exit Salida tomada por el jugador
         * @return La nueva sala
         */
        Room * changeRoom ( const Direction& exit ) ;

        /**
         * Reinicia la sala activa, es decir, se vuelve a crear la sala activa en el estado inicial
         * @return La nueva sala
         */
        Room * restartRoom () ;

        /**
         * Create room by data from file
         * @return La sala creada ó 0 si no se pudo construir
         */
        Room * createRoom ( const std::string& fileName ) ;

        Room * createRoomThenAddItToListOfRooms ( const std::string& fileName ) ;

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

        /**
         * Actualiza el jugador activo de la sala activa. El valor se obtiene a partir de
         * los datos de la sala activa
         */
        void updateActivePlayer () ;

        /**
         * Lee del disco las salas visitadas por los jugadores
         * @param visitedSequence Estructura de datos empleada por el archivo XML para guardar
         * las salas visitadas
         */
        void loadVisitedSequence ( sgxml::exploredRooms::visited_sequence& visitedSequence ) ;

        /**
         * Guarda en disco las salas visitadas por los jugadores
         * @param visitedSequence Estructura de datos empleada por el archivo XML para guardar
         * las salas visitadas
         */
        void saveVisitedSequence ( sgxml::exploredRooms::visited_sequence& visitedSequence ) ;

        /**
         * Cuenta el número de salas visitadas por los jugadores
         * @return Un número mayor que 2
         */
        unsigned short countVisitedRooms () ;

        /**
         * Busca los datos de una sala en el mapa en la lista
         * @param room El nombre del archivo de la sala
         * @return Un registro con los datos de la sala en el mapa ó 0 si la búsqueda fracasó
         */
        MapRoomData* findRoomData ( const std::string& room ) ;

        /**
         * Busca una sala en el conjunto de salas creadas
         * @param room El nombre del archivo de la sala
         * @return Una sala ó 0 si la búsqueda fracasó
         */
        Room* findRoom ( const std::string& room ) ;

        /**
         * Look for player’s way into this room
         * @param room Name of file for room
         * @param name Name of player
         * @return Position of entrance into the room
         */
        PlayerInitialPosition* findPlayerPosition ( const std::string& room, const std::string& name ) ;

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
        std::list < MapRoomData > mapData ;

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


class EqualMapRoomData : public std::binary_function< MapRoomData, std::string, bool >
{

public:
        bool operator()( const MapRoomData& mapData, const std::string& room ) const;

};

class EqualRoom : public std::binary_function< Room*, std::string, bool >
{

public:
        bool operator()( Room* room, const std::string& nameOfRoom ) const;

};

}

#endif
