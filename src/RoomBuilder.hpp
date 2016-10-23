// The free and open source remake of Head over Heels
//
// Copyright © 2016 Douglas Mencken dougmencken @ gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef RoomBuilder_hpp_
#define RoomBuilder_hpp_

#include <string>
#include <iostream>
#include <sstream>
#include <allegro.h>
#include "csxml/RoomXML.hpp"
#include "Ism.hpp"

namespace isomot
{

// Declaraciones adelantadas
class ItemDataManager;
class ItemData;
class Room;
class FloorTile;
class Wall;
class GridItem;
class FreeItem;
class PlayerItem;
class Door;

/**
 * Creador de una sala. Construye las distintas partes de una sala a partir de los datos de un archivo
 */
class RoomBuilder
{

public:

        /**
         * Constructor
         * @param itemDataManager Gestor de datos de los elementos del juego
         * @param fileName Nombre del archivo que contiene los datos de la sala
         */
        RoomBuilder(ItemDataManager* itemDataManager, const std::string& fileName);

        /**
         * Constructor para una sala que ya ha sido creada
         * @param itemDataManager Gestor de datos de los elementos del juego
         */
        RoomBuilder(ItemDataManager* itemDataManager);

        virtual ~RoomBuilder();

        /**
         * Construye la sala a partir de los datos del archivo XML
         * @return Una sala ó 0 si la sala no se pudo construir
         */
        Room* buildRoom();

        /**
         * Crea un jugador en la sala en construcción. La sala ya debe estar construida
         * @param playerId Identificador del jugador
         * @param behaviorId Comportamiento del jugador
         * @param x Coordenada isométrica X donde se situará al jugador
         * @param y Coordenada isométrica Y donde se situará al jugador
         * @param z Coordenada isométrica Z donde se situará al jugador
         * @param direction Dirección inicial del jugador
         */
        PlayerItem* buildPlayer( const PlayerId& playerId, const BehaviorId& behaviorId, int x, int y, int z, const Direction& direction );

        /**
         * Crea un jugador en la sala especificada. La sala ya debe estar construida
         * @param room La sala donde se creará el jugador
         * @param playerId Identificador del jugador. Puede cambiarse si se pretende crear al jugador compuesto
         * cuando uno de los jugadores simples ya ha agotado sus vidas
         * @param behaviorId Comportamiento del jugador
         * @param x Coordenada isométrica X donde se situará al jugador
         * @param y Coordenada isométrica Y donde se situará al jugador
         * @param z Coordenada isométrica Z donde se situará al jugador
         * @param direction Dirección inicial del jugador
         * @param hasItem Indica si el jugador llevaba un elemento en el bolso
         */
        PlayerItem* buildPlayer( Room* room, const PlayerId& playerId, const BehaviorId& behaviorId, int x, int y, int z, const Direction& direction, bool hasItem = false );

        static int getXCenterOfRoom( ItemData* playerData, Room* theRoom );

        static int getYCenterOfRoom( ItemData* playerData, Room* theRoom );

private:

        /**
         * Crea una loseta
         * @param Datos del archivo XML para crear la loseta en la sala
         */
        FloorTile* buildFloorTile( const rxml::tile& tile );

        /**
         * Crea un segmento de muro
         * @param Datos del archivo XML para crear el segmento de muro en la sala
         */
        Wall* buildWall( const rxml::wall& wall );

        /**
         * Crea un elemento rejilla
         * @param Datos del archivo XML para crear el elemento rejilla en la sala
         */
        GridItem* buildGridItem( const rxml::item& item );

        /**
         * Crea un elemento libre
         * @param Datos del archivo XML para crear el elemento libre en la sala
         */
        FreeItem* buildFreeItem( const rxml::item& item );

        /**
         * Crea una puerta
         * @param Datos del archivo XML para crear la puerta en la sala
         */
        Door* buildDoor( const rxml::item& item );

private:

        /**
         * Nombre del archivo XML que contiene los datos de la sala
         */
        std::string fileName;

        /**
         * Gestor de los datos invariables de los elementos del juego
         */
        ItemDataManager* itemDataManager;

        /**
         * La sala en construcción
         */
        Room* room;

};

}

#endif
