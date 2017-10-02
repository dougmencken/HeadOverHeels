// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Room_hpp_
#define Room_hpp_

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <utility>
#include "Ism.hpp"
#include "Drawable.hpp"
#include "Mediated.hpp"


namespace isomot
{

class FloorTile ;
class Wall ;
class Item ;
class GridItem ;
class FreeItem ;
class PlayerItem ;
class Door ;
class Camera ;
class ItemDataManager ;
class TripleRoomInitialPoint ;


/**
 * A room drawn in isometric perspective. It is composed of grid on which most of items are placed
 */

class Room : public Drawable, public Mediated
{

public:

       /**
        * @param roomFile Name of file with data about this room
        * @param scenery jail, blacktooth, marketplace, themoon, egyptus, penitentiary, safari, byblos
        * @param xTiles Número de losetas en el eje X
        * @param yTiles Número de losetas en el eje Y
        * @param tileSize Longitud del lado de una loseta en unidades isométricas
        * @param floor Kind of floor
        */
        Room( const std::string& roomFile, const std::string& scenery, int xTiles, int yTiles, int tileSize, const std::string& floor ) ;

        virtual ~Room( ) ;

       /**
        * Añade un nueva loseta a la sala
        * @param floorTile La nueva loseta
        */
        void addFloor ( FloorTile * floorTile ) ;

       /**
        * Añade un nuevo segmento de muro a la sala
        * @param wall El nuevo segmento de muro
        */
        void addWall ( Wall * wall ) ;

       /**
        * Añade una nueva puerta a la sala
        * @param door La nueva puerta
        */
        void addDoor ( Door * door ) ;

       /**
        * Añade un nuevo elemento rejilla de la sala
        */
        void addGridItem ( GridItem * gridItem ) ;

       /**
        * Añade un nuevo elemento libre de la sala
        */
        void addFreeItem ( FreeItem * freeItem ) ;

        bool addPlayerToRoom ( PlayerItem * playerItem, bool playerEntersRoom ) ;

       /**
        * Elimina una loseta de la sala
        * @param floorTile La loseta a eliminar
        */
        void removeFloor ( FloorTile * floorTile ) ;

       /**
        * Elimina un elemento rejilla de la sala
        * @param gridItem Un elemento rejilla
        */
        void removeGridItem ( GridItem * gridItem ) ;

       /**
        * Elimina un elemento libre de la sala
        * @param gridItem Un elemento libre
        */
        void removeFreeItem ( FreeItem * freeItem ) ;

        bool removePlayerFromRoom ( PlayerItem* playerItem, bool playerExitsRoom ) ;

        /**
         * Removes any bar in this room
         */
        void removeBars () ;

       /**
        * Dibuja la sala
        * @param where Imagen donde se realizará el dibujo
        */
        void draw ( BITMAP* where = 0 ) ;

       /**
        * Calcula los límites reales de la sala en función de su tamaño y las puertas existentes
        */
        void calculateBounds () ;

       /**
        * Calcula las coordenadas de pantalla donde se sitúa el origen del espacio isométrico
        * @param hasNorthDoor La sala tiene una puerta al norte
        * @param hasNorthEast La sala tiene una puerta al este
        * @param deltaX Diferencia en el eje X a aplicar para ajustar la posición de la sala en pantalla
        * @param deltaY Diferencia en el eje Y a aplicar para ajustar la posición de la sala en pantalla
        */
        void calculateCoordinates ( bool hasNorthDoor, bool hasEastDoor, int deltaX, int deltaY ) ;

        bool activatePlayerByName ( const std::string& player ) ;

       /**
        * Activa la sala. Implica la puesta en marcha de la actualización de los elementos y del dibujado
        */
        void activate () ;

       /**
        * Desactiva la sala. Implica la suspensión de la actualización de los elementos y del dibujado
        */
        void deactivate () ;

        bool swapPlayersInRoom ( ItemDataManager * itemDataManager ) ;

        bool continueWithAlivePlayer ( ) ;

       /**
        * Calculate coordinates at which player enters room
        * @param entry Way of entry
        * @param widthX Size of player on X
        * @param widthY Size of player on Y
        * @param northBound North limit of room, it is X coordinate of north walls or north door
        * @param eastBound East limit of room, it is Y coordinate of east walls or east door
        * @param southBound South limit of room, it’s X coordinate of south walls or south door
        * @param westBound West limit of room, it’s Y coordinate where are west walls or where’s west door
        * @param x Resulting X coordinate to get
        * @param y Resulting Y coordinate to get
        * @param z Resulting Z coordinate to get
        * @return true if coordinates are okay or false otherwise
        */
        bool calculateEntryCoordinates ( const Direction& entry,
                                                int widthX, int widthY,
                                                int northBound, int eastBound, int southBound, int westBound,
                                                int * x, int * y, int * z ) ;

       /**
        * Almacena una posición inicial de la cámara en una sala triple
        * @param direction Vía de entrada del jugador
        * @param x Diferencia que se debe aplicar a la coordenada de pantalla X para centrar la cámara
        * @param y Diferencia que se debe aplicar a la coordenada de pantalla Y para centrar la cámara
        */
        void addTripleRoomInitialPoint ( const Direction& direction, int x, int y ) ;

       /**
        * Almacena los límites para el desplazamiento de la cámara en una sala triple
        */
        void assignTripleRoomBounds ( int minX, int maxX, int minY, int maxY ) ;

       /**
        * Busca una posición inicial de la cámara en una sala triple
        * @param direction Vía de entrada del jugador
        */
        TripleRoomInitialPoint * findInitialPointOfEntryToTripleRoom ( const Direction& direction ) ;

protected:

        void copyAnotherCharacterAsEntered ( const std::string& name ) ;

private:

        friend class Mediator ;

        std::list < PlayerItem * > playersYetInRoom ;

        std::list < const PlayerItem * > playersWhoEnteredRoom ;

        std::string nameOfFileWithDataAboutRoom ;

       /**
        * Identificador textual del escenario al que pertenece la sala:
        * jail, blacktooth, marketplace, themoon, egyptus, penitentiary, safary o byblos
        */
        std::string scenery ;

       /**
        * Coordinates X and Y where the origin of room is
        */
        std::pair < int, int > coordinates ;

       /**
        * Number of room’s tiles both in X-way and in Y-way
        */
        std::pair < unsigned int, unsigned int > numberOfTiles ;

       /**
        * Longitud del lado de una loseta en unidades isométricas
        */
        unsigned int tileSize ;

        std::string kindOfFloor ;

       /**
        * Indica si la sala está activa, es decir, si debe dibujarse
        */
        bool active ;

       /**
        * Salida por la que algún jugador abandona la sala
        */
        Direction exit ;

       /**
        * Indices in sequence of how columns of grid items are drawn, diagonally from right to left
        */
        int * drawIndex ;

       /**
        * Grado de opacidad de las sombras desde 0, sin sombras, hasta 256, sombras totalmente opacas
        */
        short shadingScale ;

       /**
        * Último identificador del motor asignado a un elemento rejilla
        */
        int lastGridId ;

       /**
        * Último identificador del motor asignado a un elemento rejilla
        */
        int lastFreeId ;

       /**
        * Coordenadas isométricas que delimitan la sala. Existe una por cada punto cardinal
        */
        unsigned short bounds[ 12 ] ;

       /**
        * El suelo de la sala formado por losetas
        */
        std::vector < FloorTile * > floor ;

       /**
        * El muro este
        */
        std::vector < Wall * > wallX ;

       /**
        * El muro norte
        */
        std::vector < Wall * > wallY ;

       /**
        * Las puertas. Se almacenan es este orden: sur, oeste, norte, este y para las salas triples:
        * noreste, sureste, noroeste, suroeste, este-norte, este-sur, oeste-norte y oeste-sur
        */
        Door * doors[ 12 ] ;

       /**
        * Cámara encargada de centrar una sala grande en pantalla
        */
        Camera * camera ;

       /**
        * Imagen donde se dibuja la sala activa
        */
        BITMAP * picture ;

        std::list < TripleRoomInitialPoint > listOfInitialPointsForTripleRoom ;

       /**
        * Límites para mover la cámara a lo largo del eje X en una sala triple
        */
        std::pair < int, int > tripleRoomBoundX ;

       /**
        * Límites para mover la cámara a lo largo del eje Y en una sala triple
        */
        std::pair < int, int > tripleRoomBoundY ;

public:

        std::list < PlayerItem * > getPlayersYetInRoom () const ;

        std::list < const PlayerItem * > getPlayersWhoEnteredRoom () const ;

        bool isAnyPlayerStillInRoom () const ;

        std::string getNameOfFileWithDataAboutRoom () const {  return nameOfFileWithDataAboutRoom ;  }

       /**
        * Identificador textual del escenario al que pertenece la sala
        * @return jail, blacktooth, marketplace, themoon, egyptus, penitentiary, safary o byblos
        */
        std::string getScenery () const {  return this->scenery ;  }

       /**
        * Screen coordinate X of room’s origin
        */
        int getX0 () const {  return coordinates.first ;  }

       /**
        * Screen coordinate Y of room’s origin
        */
        int getY0 () const {  return coordinates.second ;  }

       /**
        * Número de losetas de la sala en el eje X
        */
        unsigned int getTilesX () const {  return numberOfTiles.first ;  }

       /**
        * Número de losetas de la sala en el eje Y
        */
        unsigned int getTilesY () const {  return numberOfTiles.second ;  }

       /**
        * Returns length of side of single tile in isometric units
        */
        unsigned int getSizeOfOneTile () const {  return tileSize ;  }

       /**
        * @return kind of floor which may be "plain", "mortal", or absent "none"
        */
        std::string getKindOfFloor () const {  return kindOfFloor ;  }

       /**
        * Activa o desactiva la sala. Una sala desactivada no se dibuja
        * @return true para comenzar a dibujar la sala o false en caso contrario
        */
        void setActive ( const bool active ) {  this->active = active ;  }

       /**
        * Is this room active
        */
        bool isActive () const {  return active ;  }

       /**
        * Límite de la sala
        * @param direction Un punto cardinal indicativo del límite que se quiere obtener
        * @return Un valor en unidades isométricas
        */
        unsigned short getBound ( const Direction& direction ) const {  return bounds[ direction ] ;  }

       /**
        * Una puerta de la sala
        * @param direction Un punto cardinal que indica dónde está la puerta
        * @return Una puerta ó 0 si no hay puerta en el punto cardinal indicado
        */
        Door * getDoor ( const Direction& direction ) const ;

       /**
        * Establece la salida por la que algún jugador abandona la sala
        * @param exit El suelo, el techo o la ubicación de alguna puerta
        */
        void setWayOfExit ( const Direction& exit ) {  this->exit = exit ;  }

       /**
        * Salida por la que algún jugador abandona la sala
        * @return El suelo, el techo o la ubicación de alguna puerta
        */
        Direction getWayOfExit () const {  return exit ;  }

       /**
        * Cámara encargada de centrar una sala grande en pantalla
        */
        Camera * getCamera () const ;

       /**
        * Imagen donde se dibuja la sala activa
        * @return Un bitmap de Allegro
        */
        BITMAP * getPicture () {  return picture ;  }

       /**
        * Límites para mover la cámara a lo largo del eje X en una sala triple
        */
        std::pair < int, int > getTripleRoomBoundX () const {  return this->tripleRoomBoundX ;  }

       /**
        * Límites para mover la cámara a lo largo del eje Y en una sala triple
        */
        std::pair < int, int > getTripleRoomBoundY () const {  return this->tripleRoomBoundY ;  }

};


/**
 * Way of entry with pair of coordinates used to store initial position of camera in triple room
 */

class TripleRoomInitialPoint
{

public:

        TripleRoomInitialPoint( const Direction& wayOfEntry, int x, int y ) ;

private:

        Direction wayOfEntry ;

        /**
         * Difference to screen coordinate X to center camera
         */
        int x ;

        /**
         * Difference to screen coordinate Y to center camera
         */
        int y ;

public:

        Direction getWayOfEntry () const {  return this->wayOfEntry ;  }

        int getX () const {  return this->x ;  }

        int getY () const {  return this->y ;  }

};


class EqualTripleRoomInitialPoint : public std::binary_function< TripleRoomInitialPoint, Direction, bool >
{

public:
        bool operator()( TripleRoomInitialPoint point, Direction wayOfEntry ) const ;

};

}

#endif
