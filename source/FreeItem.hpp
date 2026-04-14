// The free and open source remake of Head over Heels
//
// Copyright © 2026 Douglas Mencken dougmencken@gmail.com
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

#include "DescribedItem.hpp"
#include "Drawable.hpp"


class DescriptionOfItem ;

/**
 * Free items may be anywhere and move within the room, such as player characters,
 * enemies, or something whose widths differ from the widths of the grid cells
 */

class FreeItem : public DescribedItem, public Drawable
{

public :

       /**
        * @param description The description of this item
        * @param x The X of the northernmost point
        * @param y The Y of the westernmost point
        * @param z The Z of the lowest point, or how far is the floor
        * @param heading The initial heading (angular orientation)
        */
        FreeItem( const DescriptionOfItem & description, int x, int y, int z, const std::string & heading = "" ) ;

        FreeItem( const FreeItem & freeItem ) ;

        virtual ~FreeItem( ) { }

        virtual std::string whichClassOfItem () const {  return "free item" ;  }

        /**
         * for sorting free items in a container
         */
        bool operator < ( const FreeItem & that ) const
        {
                return isBehind( that ) ;
        }

        virtual int getX () const {  return this->theX ;  }
        virtual int getY () const {  return this->theY ;  }
        virtual int getZ () const {  return this->theZ ;  }

        virtual void setX ( int newX ) {  this->theX = newX ;  }
        virtual void setY ( int newY ) {  this->theY = newY ;  }
        virtual void setZ ( int newZ ) {  this->theZ = newZ ;  }

        int getInitialCellX () const {  return this->initialCellX ;  }
        int getInitialCellY () const {  return this->initialCellY ;  }
        int getInitialCellZ () const {  return this->initialCellZ ;  }

        void setInitialCell ( int cx, int cy, int cz )
        {
                this->initialCellX = cx ;
                this->initialCellY = cy ;
                this->initialCellZ = cz ;
        }

        virtual bool addToPosition ( int x, int y, int z ) ;

        const std::string & getHeading () const {  return this->heading ;  }

        void changeHeading ( const std::string & where ) ;

        void reverseHeading () ;

        void toTheHeadingFrameSequence () {  setCurrentFrameSequence( getHeading() );  }

        virtual void metamorphInto ( const std::string & newKind, const std::string & causedBy )
        {
                // the new kind may not have extra frames, like for blinking
                if ( isAtExtraFrame() ) toTheHeadingFrameSequence() ;

                DescribedItem::metamorphInto( newKind, causedBy );
        }

        virtual void draw () ;

        virtual void freshProcessedImage () ;

        void freshBothProcessedImages () ;

        virtual void addFrameTo ( const std::string & sequence, NamedPicture *const frame )
        {
                DescribedItem::addFrameTo( sequence, frame );
                freshBothProcessedImages() ;
        }

        /**
         * The distance of the processed image from the room’s origin
         */
        virtual int getImageOffsetX () const
        {
                return ( ( getX() - getY() ) << 1 ) + getWidthX() + getWidthY() - ( getCurrentRawImage().getWidth() >> 1 ) - 1 ;
        }

        virtual int getImageOffsetY () const
        {
                return getX() + getY() + getWidthX() - getCurrentRawImage().getHeight() - getZ() ;
        }

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
         * Set item’s inactivity
         */
        void setFrozen ( bool frozen ) {  this->frozen = frozen ;  }

        bool isFrozen () const {  return frozen ;  }

        NamedPicture & getShadedNonmaskedImage () const {  return * shadedNonmaskedImage ;  }

        bool isPartOfDoor () const {  return getDescriptionOfItem().isPartOfDoor() ;  }

        /**
         * When the carrier moves, the item above it (this item) also moves
         */
        const std::string & getCarrier () const {  return this->carrier ;  }

        void setCarrier ( const std::string & itemBelow ) {  this->carrier = itemBelow ;  }

        static const int far_far_away = -1024 ;

protected :

        virtual void updateImage () ;

        /**
         * See if this item hits a door’s jamb, if yes it moves
         * @param at the door mentioned by its location in the room
         * @param collision the unique name of item that collides with this one
         * @param previousX the 3D X coordinate before hitting a door’s jamb
         * @param previousY the 3D Y coordinate before hitting a door’s jamb
         */
        bool isCollidingWithJamb ( const std::string & at, const std::string & collision, const int previousX, const int previousY ) ;

private :

        // the position in 3-dimensional space of this item’s lower north-west point, in free units
        int theX ;
        int theY ;
        int theZ ;

        // the initial grid cell where this item was in when the room was (re)built
        int initialCellX ;
        int initialCellY ;
        int initialCellZ ;

        // the angular orientation
        std::string heading ;

        tribool wantMask ;

        // whether this item is inactive
        bool frozen ;

        // the unique name of item below this one
        // the item above the carrier moves along with the moving carrier
        std::string carrier ;

        /**
         * Current frame of this item shaded but not masked yet
         */
        autouniqueptr< NamedPicture > shadedNonmaskedImage ;

} ;

typedef multiptr < FreeItem > FreeItemPtr ;

#endif
