// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef DescribedItem_hpp_
#define DescribedItem_hpp_

#include "AbstractItem.hpp"
#include "DescriptionOfItem.hpp"
#include "Shady.hpp"
#include "Timer.hpp"

#include <string>

class DescriptionOfItem ;


class DescribedItem : public AbstractItem, public Shady
{

private :

        const DescriptionOfItem * descriptionOfItem ;

        /**
         * The original kind of item, while the current kind may change via metamorphosis
         */
        std::string originalKind ;

        // the height, or width along the z, of this item in free units
        unsigned int height ;

        // whether to ignore that this item collides with something
        bool ignoreCollisions ;

        /**
         * The image of this item which is okay to draw together with other items.
         * It has shadows coming from the items around, and for a free item it is also masked
         */
        autouniqueptr< Picture > processedImage ;

        Timer animationTimer ;

protected :

        DescribedItem( const DescriptionOfItem & description )
                : AbstractItem( ), Shady()
                , descriptionOfItem( & description )
                , originalKind( description.getKind () )
                , height( description.getHeight() )
                , ignoreCollisions( false )
                , processedImage( new Picture( description.getWidthOfFrame(), description.getHeightOfFrame() ) )
        {
                setUniqueName( this->originalKind + "." + util::makeRandomString( 12 ) );

                readGraphicsOfItem ();

                this->animationTimer.go() ;
        }

        DescribedItem( const DescribedItem & replicateMe )
                : AbstractItem( replicateMe ), Shady( replicateMe.getWantShadow() )
                , descriptionOfItem( replicateMe.descriptionOfItem )
                , originalKind( replicateMe.originalKind )
                , height( replicateMe.height )
                , ignoreCollisions( replicateMe.ignoreCollisions )
                , processedImage( new Picture( * replicateMe.processedImage ) )
        {
                this->animationTimer.go() ;
        }

        virtual void updateImage () {  freshProcessedImage() ;  }

        virtual void updateShadow () { }

public :

        virtual std::string whichItemClass () const {  return "described item" ;  }

        const DescriptionOfItem & getDescriptionOfItem () const {  return * this->descriptionOfItem ;  }

        const std::string & getKind () const {  return getDescriptionOfItem().getKind () ;  }

        /**
         * Gives the original kind of item, while the current kind may change via metamorphosis
         */
        const std::string & getOriginalKind () const {  return this->originalKind ;  }

        /**
         * Metamorph into another kind of item, such as into bubbles when a character teleports
         */
        virtual void metamorphInto ( const std::string & newKind, const std::string & initiatedBy ) ;

        bool isMetamorphed () const {  return getKind() != getOriginalKind() ;  }

        /**
         * The width of this item on X in free units
         */
        unsigned int getUnsignedWidthX () const {  return getDescriptionOfItem().getWidthX () ;  }
        int getWidthX () const {  return static_cast< int >( getUnsignedWidthX () );  }

        /**
         * The width of this item on Y in free units
         */
        unsigned int getUnsignedWidthY () const {  return getDescriptionOfItem().getWidthY () ;  }
        int getWidthY () const {  return static_cast< int >( getUnsignedWidthY () );  }

        /**
         * The height, or width on Z, of this item in free units
         */
        unsigned int getUnsignedHeight () const {  return this->height ;  }
        int getHeight () const {  return static_cast< int >( getUnsignedHeight () );  }

        void changeHeightTo ( int newHeight ) {  this->height = newHeight ;  }

        bool isMortal () const {  return getDescriptionOfItem().isMortal () ;  }

        /** Time in seconds to move item
         */
        float getSpeed () const {  return getDescriptionOfItem().getSpeed () ;  }

        /** Time in seconds to fall item
         */
        float getWeight () const {  return getDescriptionOfItem().getWeight() ;  }

        /**
         * Time in seconds between each frame of item’s animation
         */
        float getDelayBetweenFrames () const {  return getDescriptionOfItem().getDelayBetweenFrames() ;  }

        // the position in 3-dimensional space of this item’s lower north-west point, in free units
        virtual int getX () const = 0 ;
        virtual int getY () const = 0 ;
        virtual int getZ () const = 0 ;

        virtual void setX ( int newX ) = 0 ;
        virtual void setY ( int newY ) = 0 ;
        virtual void setZ ( int newZ ) = 0 ;

        /**
         * May this item advance to the given offset or not
         * @return true if that location is free, or false if there’s a collision,
         *         in the latter case the colliding items are collected
         */
        virtual bool canAdvanceTo ( int dx, int dy, int dz ) ;

        bool isBehind ( const DescribedItem & item ) const
        {
                return isBehindAt( item, item.getX(), item.getY(), item.getZ() );
        }

        bool isBehindAt ( const DescribedItem & item, int x, int y, int z ) const
        {
                return  ( getZ() < z + item.getHeight() ) &&
                        ( getX() < x + item.getWidthX() ) &&
                        ( getY() - getWidthY() < y ) ;
        }

        bool overlapsWith ( const DescribedItem & anotherItem ) const ;

        void setIgnoreCollisions ( bool ignore ) {  this->ignoreCollisions = ignore ;  }

        bool isIgnoringCollisions () const {  return this->ignoreCollisions ;  }
        bool isNotIgnoringCollisions () const {  return ! this->ignoreCollisions ;  }

        Picture & getProcessedImage () const {  return * this->processedImage ;  }

        virtual void freshProcessedImage () ;

        virtual void animate () ;

        virtual bool isAnimated () const
        {
                // an item with more than one frame per orientation is animated
                return getDescriptionOfItem().howManyFramesPerOrientation() > 1 ;
        }

        virtual bool isAnimationFinished () const
        {
                return isAtExtraFrame() ? true : AbstractItem::isAnimationFinished() ;
        }

private :

        void readGraphicsOfItem () ;

        /**
         * Extract frames for this item from the picture file
         */
        void createFrames () ;

        /**
         * Extract frames for the shadow of this item from the picture file
         */
        void createShadowFrames () ;

        bool isAtExtraFrame () const {  return getCurrentFrameSequence() == "extra" ;  }

} ;

typedef multiptr < DescribedItem > DescribedItemPtr ;

#endif
