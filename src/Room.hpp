// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
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
#include <map>

#include "Ism.hpp"
#include "Way.hpp"
#include "Drawable.hpp"
#include "Mediated.hpp"
#include "Door.hpp"


namespace isomot
{

class FloorTile ;
class Wall ;
class Item ;
class GridItem ;
class FreeItem ;
class PlayerItem ;
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
        * @param xTiles How many tiles on X
        * @param yTiles How many tiles on Y
        * @param tileSize Length of side of single tile
        * @param floor Kind of floor
        */
        Room( const std::string& roomFile, const std::string& scenery, int xTiles, int yTiles, int tileSize, const std::string& floor ) ;

        virtual ~Room( ) ;

        void addFloor ( FloorTile * floorTile ) ;

        void addWall ( Wall * wall ) ;

        void addDoor ( Door * door ) ;

        void updateWallsWithDoors () ;

        void addGridItem ( GridItem * gridItem ) ;

        void addFreeItem ( FreeItem * freeItem ) ;

        bool addPlayerToRoom ( PlayerItem * playerItem, bool playerEntersRoom ) ;

        void removeFloor ( FloorTile * floorTile ) ;

        void removeGridItem ( GridItem * gridItem ) ;

        void removeFreeItem ( FreeItem * freeItem ) ;

        bool removePlayerFromRoom ( PlayerItem * playerItem, bool playerExitsRoom ) ;

        /**
         * Removes any bar in this room
         * @return how many bars are gone
         */
        unsigned int removeBars () ;

        /**
         * After that, no item in this room would disappear on jump
         */
        void dontDisappearOnJump () ;

        void draw ( BITMAP * where ) ;

        void drawRoom () {  draw( whereToDraw ) ;  }

       /**
        * Calculate boundaries of room from its size and its doors
        */
        void calculateBounds () ;

       /**
        * Calculate coordinates on screen of isometric space’s origin
        * @param hasNorthDoor when room has door at north
        * @param hasNorthEast when room has door at east
        */
        void calculateCoordinates ( bool hasNorthDoor, bool hasEastDoor ) ;

        bool activateCharacterByLabel ( const std::string& player ) ;

        void activate () ;

       /**
        * Suspend updating items and drawing
        */
        void deactivate () ;

        bool swapCharactersInRoom ( ItemDataManager * itemDataManager ) ;

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
        bool calculateEntryCoordinates ( const Way& wayOfEntry,
                                                int widthX, int widthY,
                                                int northBound, int eastBound, int southBound, int westBound,
                                                int * x, int * y, int * z ) ;

       /**
        * Almacena una posición inicial de la cámara en una sala triple
        * @param x Diferencia que se debe aplicar a la coordenada de pantalla X para centrar la cámara
        * @param y Diferencia que se debe aplicar a la coordenada de pantalla Y para centrar la cámara
        */
        void addTripleRoomInitialPoint ( const Way& way, int x, int y ) ;

       /**
        * Almacena los límites para el desplazamiento de la cámara en una sala triple
        */
        void assignTripleRoomBounds ( int minX, int maxX, int minY, int maxY ) ;

       /**
        * Busca una posición inicial de la cámara en una sala triple
        */
        TripleRoomInitialPoint * findInitialPointOfEntryToTripleRoom ( const Way& way ) ;

protected:

        void copyAnotherCharacterAsEntered ( const std::string& name ) ;

        void dumpItemInsideThisRoom ( const Item* item ) ;

private:

        friend class Mediator ;

        std::list < PlayerItem * > playersYetInRoom ;

        std::list < const PlayerItem * > playersWhoEnteredRoom ;

        /**
         * Pairs label of item with next number for such item,
         * it’s used to uniquely name each item in room to deal with collisions
         */
        std::map < std::string , unsigned int > nextNumbers ;

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
        * Length of tile’s side
        */
        unsigned int tileSize ;

        std::string kindOfFloor ;

       /**
        * Is this room active, that is the one to draw
        */
        bool active ;

       /**
        * Way by which player leaves room
        */
        std::string wayOfExit ;

       /**
        * Indices in sequence of how columns of grid items are drawn, diagonally from right to left
        */
        int * drawSequence ;

       /**
        * Degree of shadows’ opacity
        * from 0, without shadows, up to 256, fully opaque shadows
        */
        short shadingScale ;

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
        * Las puertas
        */
        std::map < std::string, Door * > doors ;

       /**
        * Isometric coordinates that limit this room
        */
        std::map < std::string, unsigned short > bounds ;

       /**
        * Cámara encargada de centrar una sala grande en pantalla
        */
        Camera * camera ;

       /**
        * Where to draw active room
        */
        BITMAP * whereToDraw ;

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

        bool isSingleRoom () const {  return getTilesX() <= 10 && getTilesY() <= 10 ;  }

       /**
        * Returns length of side of single tile in isometric units
        */
        unsigned int getSizeOfOneTile () const {  return tileSize ;  }

       /**
        * @return kind of floor which may be "plain", "mortal", or absent "none"
        */
        std::string getKindOfFloor () const {  return kindOfFloor ;  }

        bool isActive () const {  return active ;  }

        unsigned short getLimitAt ( const std::string& way ) {  return bounds[ way ] ;  }

        Door * getDoorAt ( const std::string& way ) {  return doors[ way ] ;  }

        bool hasDoorAt ( const std::string& way ) {  return ( doors[ way ] != nilPointer ) ;  }

        void setWayOfExit ( const std::string& exit ) {  this->wayOfExit = exit ;  }

        std::string getWayOfExit () const {  return wayOfExit ;  }

        Camera * getCamera () const {  return camera ;  }

        BITMAP * getWhereToDraw () {  return whereToDraw ;  }

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

        TripleRoomInitialPoint( const Way& way, int x, int y ) ;

private:

        Way wayOfEntry ;

        /**
         * Offset of camera to screen X
         */
        int x ;

        /**
         * Offset of camera to screen Y
         */
        int y ;

public:

        Way getWayOfEntry () const {  return this->wayOfEntry ;  }

        int getX () const {  return this->x ;  }

        int getY () const {  return this->y ;  }

};


class EqualTripleRoomInitialPoint : public std::binary_function< TripleRoomInitialPoint, Way, bool >
{

public:
        bool operator()( TripleRoomInitialPoint point, const Way& wayOfEntry ) const ;

};

}

#endif
