// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Room_hpp_
#define Room_hpp_

#include <string>
#include <vector>
#include <set>
#include <map>

#include "Way.hpp"
#include "Picture.hpp"
#include "Drawable.hpp"
#include "Mediated.hpp"
#include "DescribedItem.hpp"
#include "GridItem.hpp"
#include "FreeItem.hpp"
#include "AvatarItem.hpp"
#include "Door.hpp"
#include "ConnectedRooms.hpp"

class FloorTile ;
class WallPiece ;
class Camera ;


/**
 * A game room
 */

class Room : public Drawable, public Mediated
{

public:

        /**
         * @param roomFile the name of file with the description of this room
         * @param xTiles the length along X, in tiles
         * @param yTiles the length along Y, in tiles
         * @param roomScenery the scenery such as moon or safari
         * @param floorKind the kind of floor
         */
        Room( const std::string & roomFile, unsigned short xTiles, unsigned short yTiles,
                        const std::string & roomScenery, const std::string & floorKind ) ;

        virtual ~Room( ) ;

        bool isSingleRoom () const {  return getTilesOnX() <= maxTilesOfSingleRoom && getTilesOnY() <= maxTilesOfSingleRoom ;  }

        bool isTripleRoom () const {  return getTilesOnX() > maxTilesOfSingleRoom && getTilesOnY() > maxTilesOfSingleRoom ;  }

        bool isDoubleRoomAlongX () const {  return getTilesOnX() > maxTilesOfSingleRoom && getTilesOnY() <= maxTilesOfSingleRoom ;  }
        bool isDoubleRoomAlongY () const {  return getTilesOnX() <= maxTilesOfSingleRoom && getTilesOnY() > maxTilesOfSingleRoom ;  }

        bool saveAsXML ( const std::string& file ) ;

        void addFloorTile ( FloorTile * floorTile ) ;

        void addWallPiece ( WallPiece * segment ) ;

        void addDoor ( Door * door ) ;

        /**
         * convert walls near doors to grid items to draw them after doors
         */
        void convertWallsNearDoors () ;

        void addGridItem ( const GridItemPtr & gridItem ) ;

        /**
         * sorts each column of grid items in such a way that
         * the next item’s Z is greater than the preceding item’s Z
         */
        void sortGridItems () ;

        void sortFreeItems () ;

        void addFreeItem ( const FreeItemPtr & freeItem ) ;

        /**
         * Places the character in this room
         * @param justEntered true to copy the character for restoring when the room restarts
         * @return true if the character is placed in the room or has already been there
         */
        bool placeCharacterInRoom ( const std::string & name,
                                        bool justEntered,
                                        int x, int y, int z,
                                        const std::string & heading,
                                        const std::string & wayOfEntry = "" );

        bool placeCharacterInRoom ( const AvatarItemPtr & character, bool justEntered ) ;

        void removeFloorAt ( int tileX, int tileY ) ;

        void removeFloorTile ( FloorTile * floorTile ) ;

        void removeWallPiece ( WallPiece * segment ) ;

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
         * Calculate boundaries of room from its size and doors
         */
        void calculateBounds () ;

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
        bool calculateEntryCoordinates ( const std::string & way,
                                                int widthX, int widthY,
                                                int previousNorthBound, int previousEastBound, int previousSouthBound, int previousWestBound,
                                                int * x, int * y, int * z ) ;

        const ConnectedRooms * getConnections () const {  return connections ;  }

        void setConnections ( const ConnectedRooms * links ) {  connections = links ;  }

        unsigned short getTransparencyOfShadows () const {  return this->shadingTransparency ;  }

        const std::vector < AvatarItemPtr > & getCharactersYetInRoom () const {  return charactersYetInRoom ;  }

        const std::vector < AvatarItemPtr > & getCharactersWhoEnteredRoom () const {  return charactersWhoEnteredRoom ;  }

        bool isAnyCharacterStillInRoom () const ;

        const std::string & getNameOfRoomDescriptionFile () const {  return this->nameOfRoomDescriptionFile ;  }

        /**
         * @return one of blacktooth, jail, market, moon, byblos, egyptus, penitentiary, safari
         */
        const std::string & getScenery () const {  return this->scenery ;  }

        const std::string & getColor () const {  return this->color ;  }

        void setColor ( const std::string & roomColor ) {  this->color = roomColor ;  }

        /**
         * the X coordinate of the room’s origin point
         */
        int getX0 () const {  return getTilesOnY () * ( getSizeOfOneTile () << 1 ) ;  }

        /**
         * the Y coordinate of the room’s origin point
         */
        int getY0 () const {  return ( Room::MaxLayers + 2 ) * Room::LayerHeight ;  }

        unsigned int getWidthOfRoomImage () const ;

        unsigned int getHeightOfRoomImage () const ;

        unsigned short getTilesOnX () const {  return this->howManyTilesOnX ;  }

        unsigned short getTilesOnY () const {  return this->howManyTilesOnY ;  }

        /**
         * The length of the single tile's side, in isometric units
         */
        static const unsigned int Single_Tile_Size = 16 ;

        virtual // inherit as subclass for other sizes but 16
        unsigned int getSizeOfOneTile () const {  return Single_Tile_Size ;  }

        /**
         * The kind of floor which may be "plain", "mortal", or "absent"
         */
        const std::string & getKindOfFloor () const {  return this->kindOfFloor ;  }

        bool hasFloor () const {  return this->kindOfFloor != "absent" ;  }

        short getLimitAt ( const std::string& way ) {  return bounds[ way ] ;  }

        const std::set < std::pair < int, int > > & getTilesWithoutFloor () const {  return this->tilesWithoutFloor ;  }

        void setTilesWithoutFloor ( const std::set < std::pair < int, int > > & noFloor ) {  this->tilesWithoutFloor = noFloor ;  }

        const std::vector < std::vector < GridItemPtr > > & getGridItems () const {  return this->gridItems ;  }

        const std::vector < FreeItemPtr > & getFreeItems () const {  return this->freeItems ;  }

        Door * getDoorOn ( const std::string & side ) const
        {
                std::map< std::string, Door * >::const_iterator it = doors.find( side );
                return it != doors.end() ? it->second : nilPointer ;
        }

        bool hasDoorOn ( const std::string & side ) const {  return getDoorOn( side ) != nilPointer ;  }

        const std::map < std::string, Door * > & getDoors () const {  return this->doors ;  }

        FloorTile * getFloorTileAtColumn ( unsigned int column ) const {  return this->floorTiles[ column ] ;  }

        Camera * getCamera () const {  return camera ;  }

        PicturePtr getWhereToDraw () const {  return whereToDraw ;  }

        int getXCenterForItem ( int widthX ) ;

        int getYCenterForItem ( int widthY ) ;

private:

        void copyAnotherCharacterAsEntered ( const std::string& name ) ;

        void dumpItemInsideThisRoom ( const DescribedItem & item ) ;

protected:

        void addGridItemToContainer ( const GridItemPtr & gridItem ) ;

        void addFreeItemToContainer ( const FreeItemPtr & freeItem ) ;

public:

        /**
         * The "z" position of the floor
         */
        static const int FloorZ = -1 ;

        /**
         * The height in isometric units of a layer
         * Item in the grid at height n is n * LayerHeight units
         */
        static const int LayerHeight = 24 ;

        /**
         * The maximum number of layers in a room
         * In isometric units, the maximum height of room is LayerHeight * MaxLayers
         */
        static const int MaxLayers = 10 ;

        /**
         * the rooms larger than this number of tiles are not "single"
         */
        static const unsigned int maxTilesOfSingleRoom = 10 ;

        static const unsigned int Sides = 12 ;
        static const std::string Sides_Of_Room [] ;

private:

        // in which file is this room described
        std::string nameOfRoomDescriptionFile ;

        // how big is this room in tiles
        unsigned short howManyTilesOnX ;
        unsigned short howManyTilesOnY ;

        std::string scenery ;

        std::string kindOfFloor ;

        std::string color ;

        // the connections of this room with other rooms on the map
        const ConnectedRooms * connections ;

        std::vector < AvatarItemPtr > charactersYetInRoom ;

        std::vector < AvatarItemPtr > charactersWhoEnteredRoom ;

        /**
         * Pairs the kind of item with the next number for such items,
         * it’s used to name the every item in a room uniquely
         */
        std::map < std::string , unsigned int > nextNumbers ;

        /**
         * The sequence of drawing for columns of grid items
         */
        unsigned int * drawingSequence ;

        /**
         * The transparency of shadows
         * from 0 for pure black shadows, thru 128 for 50% opacity, up to 256 for no shadows
         */
        unsigned short shadingTransparency ;

        std::set < std::pair < int, int > > tilesWithoutFloor ;

        std::vector < FloorTile * > floorTiles ;

        std::vector < WallPiece * > wallPieces ;

        // the grid items
        std::vector < std::vector < GridItemPtr > > gridItems ;

        // the free items in this room
        std::vector < FreeItemPtr > freeItems ;

        // the doors
        std::map < std::string, Door * > doors ;

        /**
         * Isometric coordinates that limit this room
         */
        std::map < std::string, short > bounds ;

        Camera * camera ;

        PicturePtr whereToDraw ;

} ;

#endif
