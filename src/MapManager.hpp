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


/**
 * El gestor del mapa
 */

class MapManager
{

public:

        /**
         * Constructor
         * @param isomot El motor isométrico
         * @param fileName Nombre del archivo XML que contiene los datos del mapa
         */
        MapManager( Isomot* isomot, const std::string& fileName ) ;

        virtual ~MapManager( ) ;

        /**
         * Compose data for map from XML file
         */
        void loadMap () ;

        virtual void beginNewGame ( const std::string& firstRoomFileName, const std::string& secondRoomFileName ) ;

        void beginOldGameWithPlayer ( const sgxml::player& data ) ;

        /**
         * Limpia los datos almacenados relacionados con la partida en curso
         */
        void reset () ;

        /**
         * Cambia la sala activa en función de la salida tomada por el jugador
         * @param exit Salida tomada por el jugador
         * @return La nueva sala
         */
        Room* changeRoom ( const Direction& exit ) ;

        /**
         * Reinicia la sala activa, es decir, se vuelve a crear la sala activa en el estado inicial
         * @return La nueva sala
         */
        Room* restartRoom () ;

        /**
         * Construye una sala a partir de los datos de un archivo
         * @param fileName Nombre del archivo que contiene los datos de la sala
         * @return La sala creada ó 0 si no se pudo construir
         */
        Room* createRoom ( const std::string& fileName ) ;

        /**
         * Cambia la sala activa por la sala siguiente donde haya un jugador
         */
        Room* swapRoom () ;

        /**
         * Destruye la sala actual y establece como sala activa la sala donde se encuentra el
         * otro jugador. Utilizada cuando uno de los jugadores se queda sin vidas
         */
        Room* destroyAndSwapRoom () ;

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
         * Busca la vía de entrada de un jugador a una sala
         * @param room El nombre del archivo de la sala
         * @param playerId Un jugador
         * @return La dirección de entrada a la sala
         */
        PlayerStartPosition* findPlayerPosition ( const std::string& room, const WhichPlayer& playerId ) ;

protected:

        /**
         * El motor isométrico
         */
        Isomot* isomot ;

        /**
         * Las salas activas
         */
        std::vector< Room* > rooms ;

        /**
         * La sala que se está mostrando en pantalla
         */
        Room* activeRoom ;

        /**
         * Nombre del archivo XML que contiene los datos del mapa
         */
        std::string fileName ;

        /**
         * Datos de todas las salas en el mapa. Almacena la comunicación entre ellas
         */
        std::list < MapRoomData > mapData ;

public:

        /**
         * La sala que se está mostrando en pantalla
         * @return Una sala
         */
        Room * getActiveRoom () const {  return activeRoom ;  }

        /**
         * La sala donde se encuentra el jugador que no está activo
         * @return Una sala ó 0 si no hay más jugadores
         */
        Room * getHideRoom () ;

};


/**
 * Objeto-función usado como predicado en la búsqueda de los datos de una sala en el mapa
 */
class EqualMapRoomData : public std::binary_function< MapRoomData, std::string, bool >
{

public:
        bool operator()( const MapRoomData& mapData, const std::string& room ) const;

};

/**
 * Objeto-función usado como predicado en la búsqueda de una sala
 */
class EqualRoom : public std::binary_function< Room*, std::string, bool >
{

public:
        bool operator()( Room* room, const std::string& identifier ) const;

};

}

#endif
