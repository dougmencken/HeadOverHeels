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
        * @param roomFile the name of file with data about this room
        * @param scenery the one of jail, blacktooth, market, moon, egyptus, penitentiary, safari, byblos
        * @param xTiles how many tiles on X
        * @param yTiles how many tiles on Y
        * @param floorKind the kind of floor
        */
        Room( const std::string & roomFile, const std::string & scenery,
                        unsigned int xTiles, unsigned int yTiles,
                                const std::string & floorKind ) ;

        virtual ~Room( ) ;

        bool isSingleRoom () const {  return getTilesX() <= maxTilesOfSingleRoom && getTilesY() <= maxTilesOfSingleRoom ;  }

        bool isTripleRoom () const {  return getTilesX() > maxTilesOfSingleRoom && getTilesY() > maxTilesOfSingleRoom ;  }

        bool isDoubleRoomAlongX () const {  return getTilesX() > maxTilesOfSingleRoom && getTilesY() <= maxTilesOfSingleRoom ;  }
        bool isDoubleRoomAlongY () const {  return getTilesX() <= maxTilesOfSingleRoom && getTilesY() > maxTilesOfSingleRoom ;  }

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
         * Removes all items of the desired kind in this room
         * @return how many items were removed
         */
        unsigned int removeItemsOfKind ( const std::string & kind ) ;

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

        /**
         * @return true if the character with this name is found in the room and activated
         */
        bool activateCharacterByName ( const std::string & name ) ;

        void activate () ;

        void deactivate () ;

        bool isActive () const ;

        bool swapCharactersInRoom () ;

        bool continueWithAliveCharacter () ;

        /**
         * Calculate coordinates at which the character enters the room
         *
         * @param the way of entry
         * @param widthX the size of the character on X
         * @param widthY the size of the character on Y
         * @param previousNorthBound the limit of the previous room in the north, it is the X coordinate of the north walls or door
         * @param previousEastBound the limit of the previous room in the east, it is the Y coordinate of the east walls or door
         * @param previousSouthBound the limit of the previous room in the south, it’s the X coordinate of the south walls or door
         * @param previousWestBound the limit of the previous room in the west, it’s the Y coordinate of the west walls or door
         * @param x the X coordinate of entrance, initially the X of exit from the previous room
         * @param y the Y coordinate of entrance, initially the Y of exit from the previous room
         * @param z the Z coordinate of entrance, initially the Z of exit from the previous room
         * @return true if coordinates are okay to enter the room or false otherwise
         */
        bool calculateEntryCoordinates ( const Way & wayOfEntry,
                                                int widthX, int widthY,
                                                int previousNorthBound, int previousEastBound, int previousSouthBound, int previousWestBound,
                                                int * x, int * y, int * z ) ;

        const RoomConnections * getConnections () const {  return connections ;  }

        void setConnections ( const RoomConnections * links ) {  connections = links ;  }

        unsigned short getOpacityOfShadows () const {  return shadingOpacity ;  }

        const std::vector < AvatarItemPtr > & getCharactersYetInRoom () const {  return charactersYetInRoom ;  }

        const std::vector < AvatarItemPtr > & getCharactersWhoEnteredRoom () const {  return charactersWhoEnteredRoom ;  }

        bool isAnyCharacterStillInRoom () const ;

        const std::string & getNameOfRoomDescriptionFile () const {  return nameOfFileWithDataAboutRoom ;  }

        /**
         * @return one of jail, blacktooth, market, moon, egyptus, penitentiary, safari, byblos
         */
        const std::string & getScenery () const {  return this->scenery ;  }

        const std::string & getColor () const {  return this->color ;  }

        void setColor ( const std::string & roomColor ) {  this->color = roomColor ;  }

        /**
         * the X coordinate of the room’s origin
         */
        int getX0 () const {  return coordinatesOfOrigin.first ;  }

        /**
         * the Y coordinate of the room’s origin
         */
        int getY0 () const {  return coordinatesOfOrigin.second ;  }

        unsigned int getWidthOfRoomImage () const ;

        unsigned int getHeightOfRoomImage () const ;

        unsigned int getTilesX () const {  return howManyTiles.first ;  }

        unsigned int getTilesY () const {  return howManyTiles.second ;  }

        /**
         * The length of the single tile's side, in isometric units
         */
        static const unsigned int Single_Tile_Size = 16 ;

        virtual // inherit as subclass for other sizes but 16
        unsigned int getSizeOfOneTile () const {  return Single_Tile_Size ;  }

        /**
         * The kind of floor which may be "plain", "mortal", or "none" if absent
         */
        const std::string & getKindOfFloor () const {  return kindOfFloor ;  }

        bool hasFloor () const {  return kindOfFloor != "absent" ;  }

        short getLimitAt ( const std::string& way ) {  return bounds[ way ] ;  }

        const std::vector < std::pair < int, int > > & getTilesWithoutFloor () const {  return tilesWithoutFloor ;  }

        void setTilesWithoutFloor ( const std::vector < std::pair < int, int > > & noFloor ) {  tilesWithoutFloor = noFloor ;  }

        const std::vector < std::vector < GridItemPtr > > & getGridItems () const {  return gridItems ;  }

        const std::vector < FreeItemPtr > & getFreeItems () const {  return freeItems ;  }

        Door * getDoorAt ( const std::string & way ) const
        {
                std::map< std::string, Door * >::const_iterator it = doors.find( way );
                return it != doors.end() ? it->second : nilPointer ;
        }

        bool hasDoorAt ( const std::string & way ) const {  return getDoorAt( way ) != nilPointer ;  }

        const std::map < std::string, Door * > & getDoors () const {  return doors ;  }

        Camera * getCamera () const {  return camera ;  }

        PicturePtr getWhereToDraw () const {  return whereToDraw ;  }

        int getXCenterForItem ( const Item & item ) {  return getXCenterForItem( item.getDescriptionOfItem() ) ;  }

        int getXCenterForItem ( const DescriptionOfItem * data ) ;

        int getYCenterForItem ( const Item & item ) {  return getYCenterForItem( item.getDescriptionOfItem() ) ;  }

        int getYCenterForItem ( const DescriptionOfItem * data ) ;

        /**
         * the rooms larger than this number of tiles are not "single"
         */
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
         * Pairs the kind of item with the next number for such items,
         * it’s used to name the every item in a room uniquely
         */
        std::map < std::string , unsigned int > nextNumbers ;

        std::string nameOfFileWithDataAboutRoom ;

        std::string scenery ;

        std::string color ;

        /**
         * Where the origin of the room is
         */
        std::pair < int /* x */, int /* y */ > coordinatesOfOrigin ;

        const std::pair < unsigned int /* tilesX */, unsigned int /* tilesY */ > howManyTiles ;

        std::string kindOfFloor ;

        /**
         * The sequence of drawing columns of grid items
         */
        unsigned int * drawSequence ;

        /**
         * The opacity of shadows
         * from 0 for pure black shadows, thru 128 for 50% opacity, up to 256 for no shadows
         */
        unsigned short shadingOpacity ;

        std::vector < std::pair < int, int > > tilesWithoutFloor ;

        std::vector < FloorTile * > floorTiles ;

        std::vector < Wall * > wallX ;

        std::vector < Wall * > wallY ;

        /**
         * The set of grid items that make up the structure of the room. Each column is sorted
         * in such a way that the next item’s Z is greater than the preceding item’s Z
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
        std::map < std::string, short > bounds ;

        Camera * camera ;

        PicturePtr whereToDraw ;

} ;

#endif
