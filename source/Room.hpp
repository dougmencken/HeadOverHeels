// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
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

#include "Way.hpp"
#include "Picture.hpp"
#include "Drawable.hpp"
#include "Mediated.hpp"
#include "RoomConnections.hpp"
#include "Item.hpp"
#include "GridItem.hpp"
#include "FreeItem.hpp"
#include "AvatarItem.hpp"
#include "Door.hpp"


class FloorTile ;
class Wall ;
class Camera ;


/**
 * A room drawn in isometric perspective. It is composed of grid on which most of items are placed
 */

class Room : public Drawable, public Mediated
{

public:

       /**
        * @param roomFile Name of file with data about this room
        * @param scenery jail, blacktooth, market, moon, egyptus, penitentiary, safari, byblos
        * @param xTiles How many tiles on X
        * @param yTiles How many tiles on Y
        * @param tileSize Length of side of single tile
        * @param floorKind Kind of floor
        */
        Room( const std::string& roomFile, const std::string& scenery,
                        unsigned int xTiles, unsigned int yTiles, unsigned int tileSize,
                                const std::string& floorKind ) ;

        virtual ~Room( ) ;

        std::string whichRoom () const ;

        bool isSingleRoom () const {  return getTilesX() <= maxTilesOfSingleRoom && getTilesY() <= maxTilesOfSingleRoom ;  }

        bool saveAsXML ( const std::string& file ) ;

        void addFloorTile ( FloorTile * floorTile ) ;

        void addWall ( Wall * wall ) ;

        void addDoor ( Door * door ) ;

        void updateWallsWithDoors () ;

        void addGridItem ( const GridItemPtr & gridItem ) ;

        void addFreeItem ( const FreeItemPtr & freeItem ) ;

        bool addCharacterToRoom ( const AvatarItemPtr & character, bool characterEntersRoom ) ;

        void removeFloorAt ( int tileX, int tileY ) ;

        void removeFloorTile ( FloorTile * floorTile ) ;

        void removeWallOnX ( Wall * segment ) ;

        void removeWallOnY ( Wall * segment ) ;

        void removeGridItemByUniqueName ( const std::string & uniqueName ) ;

        void removeFreeItemByUniqueName ( const std::string & uniqueName ) ;

        bool removeCharacterFromRoom ( const AvatarItem & character, bool characterExitsRoom ) ;

        /**
         * Removes any bar in this room
         * @return how many bars are gone
         */
        unsigned int removeBars () ;

        /**
         * After that, no item in this room would disappear on jump
         */
        void dontDisappearOnJump () ;

        virtual void draw () ;

        void drawRoom () ;

       /**
        * Calculate boundaries of room from its size and its doors
        */
        void calculateBounds () ;

        void calculateCoordinatesOfOrigin ( bool hasNorthDoor, bool hasEastDoor, bool hasSouthDoor, bool hasWestDoor ) ;

        bool activateCharacterByName ( const std::string & character ) ;

        void activate () ;

        void deactivate () ;

        bool isActive () const ;

        bool swapCharactersInRoom () ;

        bool continueWithAliveCharacter () ;

       /**
        * Calculate coordinates at which the character enters the room
        * @param entry Way of entry
        * @param widthX Size of character on X
        * @param widthY Size of character on Y
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

        const RoomConnections * getConnections () const {  return connections ;  }

        void setConnections ( const RoomConnections * links ) {  connections = links ;  }

        unsigned short getOpacityOfShadows () const {  return shadingOpacity ;  }

        const std::vector < AvatarItemPtr > & getCharactersYetInRoom () const {  return charactersYetInRoom ;  }

        const std::vector < AvatarItemPtr > & getCharactersWhoEnteredRoom () const {  return charactersWhoEnteredRoom ;  }

        bool isAnyCharacterStillInRoom () const ;

        const std::string & getNameOfRoomDescriptionFile () const {  return nameOfFileWithDataAboutRoom ;  }

       /**
        * @return jail, blacktooth, market, moon, egyptus, penitentiary, safary o byblos
        */
        const std::string & getScenery () const {  return this->scenery ;  }

        const std::string & getColor () const {  return this->color ;  }

        void setColor ( const std::string & roomColor ) {  this->color = roomColor ;  }

       /**
        * Screen coordinate X of room’s origin
        */
        int getX0 () const {  return coordinatesOfOrigin.first ;  }

       /**
        * Screen coordinate Y of room’s origin
        */
        int getY0 () const {  return coordinatesOfOrigin.second ;  }

        unsigned int getWidthOfRoomImage () const ;

        unsigned int getHeightOfRoomImage () const ;

        unsigned int getTilesX () const {  return howManyTiles.first ;  }

        unsigned int getTilesY () const {  return howManyTiles.second ;  }

       /**
        * Returns length of side of single tile in isometric units
        */
        unsigned int getSizeOfOneTile () const {  return tileSize ;  }

       /**
        * @return kind of floor which may be "plain", "mortal", or absent "none"
        */
        const std::string & getKindOfFloor () const {  return kindOfFloor ;  }

        bool hasFloor () const {  return kindOfFloor != "absent" ;  }

        unsigned short getLimitAt ( const std::string& way ) {  return bounds[ way ] ;  }

        const std::vector < std::pair < int, int > > & getTilesWithoutFloor () const {  return tilesWithoutFloor ;  }

        void setTilesWithoutFloor ( const std::vector < std::pair < int, int > > & noFloor ) {  tilesWithoutFloor = noFloor ;  }

        const std::vector < std::vector < GridItemPtr > > & getGridItems () const {  return gridItems ;  }

        const std::vector < FreeItemPtr > & getFreeItems () const {  return freeItems ;  }

        Door * getDoorAt ( const std::string & way ) const
        {
                std::map< std::string, Door * >::const_iterator it = doors.find( way );
                return it != doors.end() ? it->second : nilPointer ;
        }

        bool hasDoorAt ( const std::string & way ) const {  return ( getDoorAt( way ) != nilPointer ) ;  }

        const std::map < std::string, Door * > & getDoors () const {  return doors ;  }

        Camera * getCamera () const {  return camera ;  }

        PicturePtr getWhereToDraw () const {  return whereToDraw ;  }

        int getXCenterForItem ( const Item & item ) {  return getXCenterForItem( item.getDescriptionOfItem() ) ;  }

        int getXCenterForItem ( const DescriptionOfItem * data ) ;

        int getYCenterForItem ( const Item & item ) {  return getYCenterForItem( item.getDescriptionOfItem() ) ;  }

        int getYCenterForItem ( const DescriptionOfItem * data ) ;

        /**
         * (unused) the limits for moving the camera along X in a triple room
         */
        ///std::pair < int, int > getTripleRoomCameraLimitsX () const {  return tripleRoomCameraLimitsX ;  }

        /**
         * (unused) the limits for moving the camera along Y in a triple room
         */
        ///std::pair < int, int > getTripleRoomCameraLimitsY () const {  return tripleRoomCameraLimitsY ;  }

        /**
         * Sets the limits for moving the camera in a triple room
         */
        void setTripleRoomCameraLimits ( int minX, int maxX, int minY, int maxY )
        {
                tripleRoomCameraLimitsX.first  = minX ;
                tripleRoomCameraLimitsX.second = maxX ;
                tripleRoomCameraLimitsY.first  = minY ;
                tripleRoomCameraLimitsY.second = maxY ;
        }

        static const unsigned int maxTilesOfSingleRoom = 10 ;

protected:

        void copyAnotherCharacterAsEntered ( const std::string& name ) ;

        void dumpItemInsideThisRoom ( const Item & item ) ;

        void addGridItemToContainer ( const GridItemPtr & gridItem ) ;

        void addFreeItemToContainer ( const FreeItemPtr & freeItem ) ;

private:

        friend class Mediator ;

        // the connections of this room with other rooms
        const RoomConnections * connections ;

        std::vector < AvatarItemPtr > charactersYetInRoom ;

        std::vector < AvatarItemPtr > charactersWhoEnteredRoom ;

        /**
         * Pairs label of item with next number for such item,
         * it’s used to uniquely name each item in room to deal with collisions
         */
        std::map < std::string , unsigned int > nextNumbers ;

        std::string nameOfFileWithDataAboutRoom ;

        std::string scenery ;

        std::string color ;

       /**
        * Where the origin of room is
        */
        std::pair < int /* x */, int /* y */ > coordinatesOfOrigin ;

        const std::pair < unsigned int /* tilesX */, unsigned int /* tilesY */ > howManyTiles ;

       /**
        * Length of tile’s side
        */
        const unsigned int tileSize ;

        std::string kindOfFloor ;

       /**
        * Sequence of when to draw a column of grid items
        */
        unsigned int * drawSequence ;

       /**
        * Degree of shadows’ opacity
        * from 0 for pure black shadows, thru 128 for 50% opacity of shadows, up to 256 for no shadows
        */
        unsigned short shadingOpacity ;

        std::vector < std::pair < int, int > > tilesWithoutFloor ;

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
        * The doors
        */
        std::map < std::string, Door * > doors ;

       /**
        * Isometric coordinates that limit this room
        */
        std::map < std::string, unsigned short > bounds ;

        Camera * camera ;

        PicturePtr whereToDraw ;

        // (obsolete) the limits for moving the camera for a triple room
        /*~~~*/ std::pair < int, int > tripleRoomCameraLimitsX ;
        /*~~~*/ std::pair < int, int > tripleRoomCameraLimitsY ;

} ;

#endif
