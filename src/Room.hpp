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
#include "Picture.hpp"
#include "Drawable.hpp"
#include "Mediated.hpp"
#include "RoomConnections.hpp"
#include "Item.hpp"
#include "GridItem.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "Door.hpp"


namespace iso
{

class FloorTile ;
class Wall ;
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
        * @param floorKind Kind of floor
        */
        Room( const std::string& roomFile, const std::string& scenery, unsigned int xTiles, unsigned int yTiles, unsigned int tileSize, const std::string& floorKind ) ;

        virtual ~Room( ) ;

        std::string whichRoom () const ;

        bool isSingleRoom () const {  return getTilesX() <= maxTilesOfSingleRoom && getTilesY() <= maxTilesOfSingleRoom ;  }

        bool saveAsXML ( const std::string& file ) ;

        void addFloor ( FloorTile * floorTile ) ;

        void addWall ( Wall * wall ) ;

        void addDoor ( Door * door ) ;

        void updateWallsWithDoors () ;

        void addGridItem ( const GridItemPtr & gridItem ) ;

        void addFreeItem ( const FreeItemPtr & freeItem ) ;

        bool addPlayerToRoom ( const PlayerItemPtr & playerItem, bool playerEntersRoom ) ;

        void removeFloor ( FloorTile * floorTile ) ;

        void removeGridItemByUniqueName ( const std::string & uniqueName ) ;

        void removeFreeItemByUniqueName ( const std::string & uniqueName ) ;

        bool removePlayerFromRoom ( const PlayerItem & playerItem, bool playerExitsRoom ) ;

        /**
         * Removes any bar in this room
         * @return how many bars are gone
         */
        unsigned int removeBars () ;

        /**
         * After that, no item in this room would disappear on jump
         */
        void dontDisappearOnJump () ;

        void draw ( const allegro::Pict& where ) ;

        void drawRoom () {  if ( whereToDraw != nilPointer ) draw( whereToDraw->getAllegroPict() ) ;  }

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

        bool swapCharactersInRoom () ;

        bool continueWithAlivePlayer () ;

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

        void setVisited ( bool visited ) {  this->visited = visited ;  }

        bool isVisited () const {  return visited ;  }

        const RoomConnections * getConnections () const {  return connections ;  }

        void setConnections ( const RoomConnections * links ) {  connections = links ;  }

        unsigned short getOpacityOfShadows () const {  return shadingOpacity ;  }

        std::vector < PlayerItemPtr > getPlayersYetInRoom () const ;

        std::vector < PlayerItemPtr > getPlayersWhoEnteredRoom () const ;

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

        unsigned int getTilesX () const {  return howManyTiles.first ;  }

        unsigned int getTilesY () const {  return howManyTiles.second ;  }

       /**
        * Returns length of side of single tile in isometric units
        */
        unsigned int getSizeOfOneTile () const {  return tileSize ;  }

       /**
        * @return kind of floor which may be "plain", "mortal", or absent "none"
        */
        std::string getKindOfFloor () const {  return kindOfFloor ;  }

        bool hasFloor () const {  return kindOfFloor != "absent" ;  }

        bool isActive () const {  return active ;  }

        unsigned short getLimitAt ( const std::string& way ) {  return bounds[ way ] ;  }

        const std::vector < std::vector < GridItemPtr > > & getGridItems () const {  return gridItems ;  }

        const std::vector < FreeItemPtr > & getFreeItems () const {  return freeItems ;  }

        Door * getDoorAt ( const std::string& way ) const
        {
                std::map< std::string, Door * >::const_iterator it = doors.find( way );
                return it != doors.end() ? it->second : nilPointer ;
        }

        bool hasDoorAt ( const std::string& way ) const {  return ( getDoorAt( way ) != nilPointer ) ;  }

        const std::map < std::string, Door * >& getDoors () const {  return doors ;  }

        Camera * getCamera () const {  return camera ;  }

        Picture * getWhereToDraw () {  return whereToDraw ;  }

        int getXCenterForItem ( const Item & item ) {  return getXCenterForItem( item.getDataOfItem() ) ;  }

        int getXCenterForItem ( const ItemData * data ) ;

        int getYCenterForItem ( const Item & item ) {  return getYCenterForItem( item.getDataOfItem() ) ;  }

        int getYCenterForItem ( const ItemData * data ) ;

       /**
        * Limit for movement of camera along X in triple room
        */
        std::pair < int, int > getTripleRoomBoundX () const {  return this->tripleRoomBoundX ;  }

       /**
        * Limit for movement of camera along Y in triple room
        */
        std::pair < int, int > getTripleRoomBoundY () const {  return this->tripleRoomBoundY ;  }

       /**
        * Sets limits for movement of camera in triple room
        */
        void assignTripleRoomBounds ( int minX, int maxX, int minY, int maxY ) ;

       /**
        * Sets initial position of camera in triple room
        * @param x Difference to X screen coordinate to center camera
        * @param y Difference to Y screen coordinate to center camera
        */
        void addTripleRoomInitialPoint ( const Way& way, int x, int y ) ;

       /**
        * Look for initial position of camera in triple room
        */
        TripleRoomInitialPoint findInitialPointOfEntryToTripleRoom ( const Way& way ) ;

        static const unsigned int maxTilesOfSingleRoom = 10 ;

protected:

        void copyAnotherCharacterAsEntered ( const std::string& name ) ;

        void dumpItemInsideThisRoom ( const Item & item ) ;

        void addGridItemToContainer ( const GridItemPtr & gridItem ) ;

        void addFreeItemToContainer ( const FreeItemPtr & freeItem ) ;

private:

        ///Room( const Room & /* toCopy */ ) : Drawable( ), Mediated( )  { }

        friend class Mediator ;

        /**
         * Is this room already visited by any of characters
         */
        bool visited ;

        /*
         * Connections of room with another rooms
         */
        const RoomConnections * connections ;

        std::vector < PlayerItemPtr > playersYetInRoom ;

        std::vector < PlayerItemPtr > playersWhoEnteredRoom ;

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
        * Where the origin of room is
        */
        std::pair < int /* x */, int /* y */ > coordinates ;

        std::pair < unsigned int /* tilesX */, unsigned int /* tilesY */ > howManyTiles ;

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
        * Indices in sequence of how columns of grid items are drawn, diagonally from right to left
        */
        unsigned int * drawSequence ;

       /**
        * Degree of shadows’ opacity
        * from 0 for pure black shadows, thru 128 for 50% opacity of shadows, up to 256 for no shadows
        */
        unsigned short shadingOpacity ;

        std::vector < FloorTile * > floorTiles ;

        std::vector < Wall * > wallX ;

        std::vector < Wall * > wallY ;

       /**
        * Set of grid items that form structure of room. Each column is sorted
        * thus next item’s Z is greater than preceding item’s Z
        */
        std::vector < std::vector < GridItemPtr > > gridItems ;

       /**
        * Free items in room
        */
        std::vector < FreeItemPtr > freeItems ;

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
        Picture * whereToDraw ;

        std::list < TripleRoomInitialPoint > listOfInitialPointsForTripleRoom ;

       /**
        * Límites para mover la cámara a lo largo del eje X en una sala triple
        */
        std::pair < int, int > tripleRoomBoundX ;

       /**
        * Límites para mover la cámara a lo largo del eje Y en una sala triple
        */
        std::pair < int, int > tripleRoomBoundY ;

};


/**
 * Way of entry with pair of coordinates used to store initial position of camera in triple room
 */

class TripleRoomInitialPoint
{

public:

        TripleRoomInitialPoint( const Way& way, int x, int y ) : wayOfEntry( way ), x( x ), y( y ) { }

private:

        Way wayOfEntry ;

        /**
         * Offset of camera at X
         */
        int x ;

        /**
         * Offset of camera at Y
         */
        int y ;

public:

        Way getWayOfEntry () const {  return this->wayOfEntry ;  }

        int getX () const {  return this->x ;  }

        int getY () const {  return this->y ;  }

};

}

#endif
