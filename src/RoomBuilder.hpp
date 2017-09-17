// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
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

class ItemDataManager ;
class ItemData ;
class Room ;
class FloorTile ;
class Wall ;
class GridItem ;
class FreeItem ;
class PlayerItem ;
class Door ;

/**
 * Creates a room by constructing different parts of it via data from file
 */

class RoomBuilder
{

public:

        /**
         * Constructor
         * @param itemDataManager Gestor de datos de los elementos del juego
         * @param fileName Nombre del archivo que contiene los datos de la sala
         */
        RoomBuilder( ItemDataManager* itemDataManager, const std::string& fileName ) ;

        /**
         * Constructor para una sala que ya ha sido creada
         * @param itemDataManager Gestor de datos de los elementos del juego
         */
        RoomBuilder( ItemDataManager* itemDataManager ) ;

        virtual ~RoomBuilder( ) ;

        /**
         * Construct room by data from file
         */
        Room * buildRoom () ;

        /**
         * Create player in current room
         * @param behavior Behavior of player
         * @param x Coordenada isométrica X donde se situará al jugador
         * @param y Coordenada isométrica Y donde se situará al jugador
         * @param z Coordenada isométrica Z donde se situará al jugador
         * @param direction Dirección inicial del jugador
         */
        PlayerItem * buildPlayerInTheSameRoom ( const std::string& player, const std::string& behavior, int x, int y, int z, const Direction& direction ) ;

        /**
         * Create player in given room
         * @param room Room where to create player
         * @param behavior Behavior of player
         * @param x Coordenada isométrica X donde se situará al jugador
         * @param y Coordenada isométrica Y donde se situará al jugador
         * @param z Coordenada isométrica Z donde se situará al jugador
         * @param direction Dirección inicial del jugador
         * @param withItem Carry the player an item in bag?
         */
        PlayerItem * buildPlayerInRoom ( Room* room, const std::string& player, const std::string& behavior, int x, int y, int z, const Direction& direction, bool withItem = false );

        static int getXCenterOfRoom ( ItemData* playerData, Room* theRoom ) ;

        static int getYCenterOfRoom ( ItemData* playerData, Room* theRoom ) ;

private:

        /**
         * Crea una loseta
         * @param tile data from XML file about the floor tile in the room
         * @param gfxPrefix where to search for picture of the floor tile
         */
        FloorTile* buildFloorTile ( const rxml::tile& tile, const char* gfxPrefix ) ;

        /**
         * Crea un segmento de muro
         * @param wall data from XML file about the wall segment in the room
         * @param gfxPrefix where to search for picture of the wall segment
         */
        Wall* buildWall ( const rxml::wall& wall, const char* gfxPrefix ) ;

        /**
         * Crea un elemento rejilla
         * @param Datos del archivo XML para crear el elemento rejilla en la sala
         */
        GridItem* buildGridItem ( const rxml::item& item ) ;

        /**
         * Crea un elemento libre
         * @param Datos del archivo XML para crear el elemento libre en la sala
         */
        FreeItem* buildFreeItem ( const rxml::item& item ) ;

        /**
         * Crea una puerta
         * @param Datos del archivo XML para crear la puerta en la sala
         */
        Door* buildDoor ( const rxml::item& item ) ;

private:

        /**
         * Nombre del archivo XML que contiene los datos de la sala
         */
        std::string fileName ;

        /**
         * Gestor de los datos invariables de los elementos del juego
         */
        ItemDataManager * itemDataManager ;

        /**
         * La sala en construcción
         */
        Room * room ;

};

}

#endif
