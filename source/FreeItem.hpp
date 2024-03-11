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
        * @param where The initial orientation of item
        */
        FreeItem( const DescriptionOfItem & description, int x, int y, int z, const std::string & where = "" ) ;

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

        int getInitialCellX () const {  return this->initialCellX ;  }

        int getInitialCellY () const {  return this->initialCellY ;  }

        int getInitialCellZ () const {  return this->initialCellZ ;  }

        /**
         * the initial grid cell where this item was located when the room was (re)built
         */
        void setInitialCellLocation ( int cx, int cy, int cz )
        {
                this->initialCellX = cx ;
                this->initialCellY = cy ;
                this->initialCellZ = cz ;
        }

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
         * See if this item hits a door’s jamb, if yes it moves
         * @param at the door mentioned by its location in the room
         * @param collision the unique name of item that collides with this one
         * @param previousX the 3D X coordinate before hitting a door’s jamb
         * @param previousY the 3D Y coordinate before hitting a door’s jamb
         */
        bool isCollidingWithJamb ( const std::string & at, const std::string & collision, const int previousX, const int previousY ) ;

private:

        int initialCellX ;

        int initialCellY ;

        int initialCellZ ;

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

        /**
         * Current frame of this item shaded but not masked yet
         */
        autouniqueptr< Picture > shadedNonmaskedImage ;

};

typedef multiptr < FreeItem > FreeItemPtr ;

#endif
