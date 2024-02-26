// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef FreeItem_hpp_
#define FreeItem_hpp_

#include <utility>
#include <cmath>

#include "tribool.hpp"
#include "WrappersAllegro.hpp"

#include "Item.hpp"
#include "Drawable.hpp"


class DescriptionOfItem ;

/**
 * Free items may be anywhere and move within the room, such as player characters,
 * enemies, or something whose widths differ from the widths of the grid cells
 */

class FreeItem : public Item, public Drawable
{

public:

       /**
        * @param description Description of this item
        * @param x Position on X
        * @param y Position on Y
        * @param z Position on Z, or how far is floor
        * @param way Initial orientation of item
        */
        FreeItem( const DescriptionOfItem * description, int x, int y, int z, const std::string& way ) ;

        FreeItem( const FreeItem & freeItem ) ;

        virtual ~FreeItem( ) { }

        virtual std::string whichItemClass () const {  return "free item" ;  }

        /**
         * for sorting free items in a container
         */
        bool operator < ( const FreeItem & that ) const
        {
                return isBehind( that ) ;
        }

        int getOriginalCellX () const {  return originalCellX ;  }

        int getOriginalCellY () const {  return originalCellY ;  }

        int getOriginalCellZ () const {  return originalCellZ ;  }

        void setOriginalCellX ( int x ) {  originalCellX = x ;  }

        void setOriginalCellY ( int y ) {  originalCellY = y ;  }

        void setOriginalCellZ ( int z ) {  originalCellZ = z ;  }

        virtual bool addToPosition ( int x, int y, int z ) ;

        virtual void draw () ;

        virtual void freshProcessedImage () ;

        void freshBothProcessedImages () ;

        /**
         * The distance of the processed image from the room’s origin
         */
        virtual int getImageOffsetX () const ;
        virtual int getImageOffsetY () const ;

        /**
         * Request to shade item
         */
        void requestShadow () ;

        /**
         * Request to mask this item
         */
        void requestMask () ;

        tribool getWantMask () const {  return wantMask ;  }

        void setWantMaskFalse () {  wantMask.setFalse() ;  }

        void setWantMaskTrue () {  wantMask.setTrue() ;  }

        void setWantMaskIndeterminate () {  wantMask.setIndeterminate() ;  }

        /**
         * Grado de transparencia del elemento
         * @return Un número entre 0 y 100, inclusive
         */
        unsigned char getTransparency () const {  return transparency ;  }

        /**
         * Set item’s inactivity
         */
        void setFrozen ( bool frozen ) {  this->frozen = frozen ;  }

        bool isFrozen () const {  return frozen ;  }

        Picture & getShadedNonmaskedImage () const {  return * shadedNonmaskedImage ;  }

        bool isPartOfDoor () const {  return partOfDoor ;  }

        void setPartOfDoor ( bool isPart ) {  partOfDoor = isPart ;  }

        static const int farFarAway = -1024 ;

protected:

        virtual void updateImage () ;

        virtual void updateShadow () ;

        /**
         * See if this item hits a door's jamb, if yes it moves
         * @param at the door mentioned by its location in room
         * @param what the name of bumped item
         * @param previousX the 3D X coordinate before hitting the door
         * @param previousY the 3D Y coordinate before hitting the door
         */
        bool isCollidingWithDoor ( const std::string& at, const std::string& what, const int previousX, const int previousY ) ;

        /**
         * See if item is not under that door or that door doesn’t exist
         * @param way door mentioned by its position in room
         * @return true if item is under that door
         */
        bool isNotUnderDoorAt ( const std::string& way ) ;

        bool isUnderSomeDoor () ;

        int originalCellX ;

        int originalCellY ;

        int originalCellZ ;

        tribool wantMask ;

        /**
         * Degree of item’s transparency in percentage 0 to 100
         */
        unsigned char transparency ;

        /**
         * Whether item is inactive
         */
        bool frozen ;

        bool partOfDoor ;

private:

        /**
         * Current frame of this item shaded but not masked yet
         */
        autouniqueptr< Picture > shadedNonmaskedImage ;

};

typedef multiptr < FreeItem > FreeItemPtr ;

#endif
