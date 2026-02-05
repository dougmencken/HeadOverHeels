// The free and open source remake of Head over Heels
//
// Copyright © 2026 Douglas Mencken dougmencken@gmail.com
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

        // the sequences of pictures of item’s shadow
        std::map< std::string, std::vector< NamedPicturePtr > > shadows ;

        // whether to ignore that this item collides with something
        bool ignoreCollisions ;

        /**
         * The image of this item which is okay to draw together with other items.
         * It has shadows coming from the items around, and for a free item it is also masked
         */
        autouniqueptr< NamedPicture > processedImage ;

        Timer animationTimer ;

protected :

        DescribedItem( const DescriptionOfItem & description )
                : AbstractItem( ), Shady()
                , descriptionOfItem( & description )
                , originalKind( description.getKind () )
                , height( description.getHeight() )
                , ignoreCollisions( false )
                , processedImage( new NamedPicture( description.getWidthOfFrame(), description.getHeightOfFrame() ) )
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
                , processedImage( new NamedPicture( * replicateMe.processedImage ) )
        {
                for ( std::map< std::string, std::vector< NamedPicturePtr > >::const_iterator mi = replicateMe.shadows.begin(); mi != replicateMe.shadows.end() ; ++ mi )
                        for ( std::vector< NamedPicturePtr >::const_iterator pi = mi->second.begin() ; pi != mi->second.end() ; ++ pi )
                                this->shadows[ mi->first ].push_back( NamedPicturePtr( new NamedPicture( ( * pi )->getAllegroPict () ) ) );

                this->animationTimer.go() ;
        }

        bool isAtExtraFrame () const {  return getCurrentFrameSequence() == "extra" ;  }

        virtual unsigned int firstFrame () const
        {
                return isAtExtraFrame() ? getCurrentFrame() : AbstractItem::firstFrame() ;
        }

        virtual unsigned int lastFrame () const
        {
                return isAtExtraFrame() ? getCurrentFrame() : AbstractItem::lastFrame() ;
        }

        NamedPicture & getNthShadowIn ( const std::string & sequence, unsigned int n ) const /* throws NoSuchPictureException */ ;

        virtual void clearFrames ()
        {
                AbstractItem::clearFrames() ;
                this->shadows.clear ();
        }

        virtual void updateImage () {  freshProcessedImage() ;  }

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
        virtual void metamorphInto ( const std::string & newKind, const std::string & causedBy ) ;

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

        NamedPicture & getProcessedImage () const {  return * this->processedImage ;  }

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

        const NamedPicture & getCurrentImageOfShadowIn ( const std::string & sequence ) const
                {  return getNthShadowIn( sequence, getCurrentFrame() ) ;  }

        const NamedPicture & getCurrentImageOfShadow () const
                {  return getCurrentImageOfShadowIn( getCurrentFrameSequence() ) ;  }

        virtual void addFrameTo ( const std::string & sequence, NamedPicture *const frame )
        {
                if ( sequence == "extra" )
                        AbstractItem::addFrameTo( "extra", frame );
                else {
                        unsigned int howManyOrientations = getDescriptionOfItem().howManyOrientations() ;
                        if ( howManyOrientations == 1 ) {
                                AbstractItem::addFrameTo( "south", frame );
                                AbstractItem::addFrameTo( "west", frame );
                                AbstractItem::addFrameTo( "north", frame );
                                AbstractItem::addFrameTo( "east", frame );
                        }
                        else {
                                AbstractItem::addFrameTo( sequence, frame );

                                if ( howManyOrientations == 2 ) {
                                             if ( sequence == "south" ) AbstractItem::addFrameTo( "north", frame );
                                        else if ( sequence == "west"  ) AbstractItem::addFrameTo( "east", frame );
                                }
                        }
                }
        }

        virtual void addFrameOfShadowTo ( const std::string & sequence, NamedPicture *const frame )
        {
                if ( sequence == "extra" )
                        addShadowTo( "extra", frame );
                else {
                        unsigned int howManyOrientations = getDescriptionOfItem().howManyOrientations() ;
                        if ( howManyOrientations == 1 ) {
                                addShadowTo( "south", frame );
                                addShadowTo( "west", frame );
                                addShadowTo( "north", frame );
                                addShadowTo( "east", frame );
                        }
                        else if ( ! sequence.empty() ) {
                                addShadowTo( sequence, frame );

                                if ( howManyOrientations == 2 ) {
                                             if ( sequence == "south" ) addShadowTo( "north", frame );
                                        else if ( sequence == "west"  ) addShadowTo( "east", frame );
                                }
                        }
                }
        }

        bool hasShadow () const {  return ! this->shadows.empty() ;  }

private :

        void readGraphicsOfItem () ;

        void checkFrames () ;

        void makeFrames () ;

        void makeShadowFrames () ;

        // returns the various orientations of this item’s graphics
        //
        std::vector< std::string > whatOrientations () const
        {
                unsigned int howManyOrientations = getDescriptionOfItem().howManyOrientations() ;
                std::vector< std::string > orientations ;

                if ( howManyOrientations > 0 ) {
                        orientations.push_back( "south" );

                        if ( howManyOrientations > 1 ) /* south and west */ {
                                orientations.push_back( "west" );

                                if ( howManyOrientations > 2 ) /* south, west, north, east */ {
                                        orientations.push_back( "north" );
                                        orientations.push_back( "east" );
                                }
                        }
                }

                return orientations ;
        }

        void addShadowTo ( const std::string & sequence, NamedPicture *const frame )
        {
                this->shadows[ sequence ].push_back( NamedPicturePtr( frame ) ) ;
        }

} ;

typedef multiptr < DescribedItem > DescribedItemPtr ;

#endif
