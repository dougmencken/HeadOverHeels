// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef AbstractItem_hpp_
#define AbstractItem_hpp_

#include <string>
#include <utility>

#include <WrappersAllegro.hpp>

#include "Mediated.hpp"
#include "Picture.hpp"

namespace behaviors {  class Behavior ;  }
using behaviors::Behavior ;


/**
 * The most abstract item of the game
 */

class AbstractItem : public Mediated
{

protected :

        AbstractItem( ) ;

        AbstractItem( const AbstractItem & itemToCopy ) ;

public :

        virtual ~AbstractItem( ) ;

        virtual std::string whichItemClass () const {  return "abstract item" ;  }

        const std::string & getUniqueName () const {  return this->uniqueName ;  }

        void setUniqueName ( const std::string & name ) {  this->uniqueName = name ;  }

        /**
         * For an item with behavior, update that behavior programmatically
         * @return true if the item can be updated thereafter (it didn’t disappear from the room)
         */
        virtual bool updateItem () ;

        /**
         * Add value to the X coordinate
         * @return true on change or false when there’s a collision
         */
        virtual bool addToX ( int value ) {  return addToPosition( value, 0, 0 ) ;  }

        /**
         * Add value to the Y coordinate
         * @return true on change or false when there’s a collision
         */
        virtual bool addToY ( int value ) {  return addToPosition( 0, value, 0 ) ;  }

        /**
         * Add value to the Z coordinate
         * @return true on change or false when there’s a collision
         */
        virtual bool addToZ ( int value ) {  return addToPosition( 0, 0, value ) ;  }

        /**
         * Change position of item
         * @param x Value to add to coordinate X
         * @param y Value to add to coordinate Y
         * @param z Value to add to coordinate Z
         * @return true on change or false when there’s a collision
         */
        virtual bool addToPosition ( int x, int y, int z ) = 0 ;

        bool doGraphicsOverlap ( const AbstractItem & item ) const {  return doGraphicsOverlapAt( item, item.getImageOffsetX(), item.getImageOffsetY() );  }

        bool doGraphicsOverlapAt ( const AbstractItem & item, int x, int y ) const ;

        void setBehaviorOf ( const std::string & nameOfBehavior ) ;

        const autouniqueptr< Behavior > & getBehavior () const {  return behavior ;  }

        virtual bool isAnimated () const = 0 ;

        virtual void animate () = 0 ;

        /**
         * Animate from the first to the last frame, which is by default
         */
        virtual void doForthMotion () = 0 ;

        /**
         * Animate from the last to the first frame, backwards
         */
        virtual void doBackwardsMotion () = 0 ;

        virtual bool isAnimatedBackwards () const = 0 ;

        virtual bool isAnimationFinished () const = 0 ;

        /**
         * Changes the current frame. Frames usually change when the angular orientation changes
         * or when looping in a sequence of animation. However there’re some cases when frames
         * are changed manually. As example, in the behavior of a spring stool one frame is for
         * resting and the other is for being fold
         */
        void changeFrame ( unsigned int newFrame ) ;

        const Picture & getRawImage () const {  return * getFrameAt( getCurrentFrame() ) ;  }

        Picture & getRawImageToChangeIt () const {  return * getFrameAt( getCurrentFrame() ) ;  }

        bool hasShadow () const {  return ! shadows.empty() ;  }

        const Picture & getImageOfShadow () const {  return * getShadowAt( getCurrentFrame() ) ;  }

        size_t howManyFrames () const {  return this->frames.size () ;  }

        const PicturePtr getFrameAt( unsigned int at ) const
        {
                return ( at < howManyFrames() ) ? this->frames[ at ] : PicturePtr() ;
        }

        size_t howManyShadows () const {  return this->shadows.size () ;  }

        const PicturePtr getShadowAt( unsigned int at ) const
        {
                return ( at < howManyShadows() ) ? this->shadows[ at ] : PicturePtr() ;
        }

        /**
         * The distance of the processed image from the room’s origin
         */
        virtual int getImageOffsetX () const = 0 ;
        virtual int getImageOffsetY () const = 0 ;

        /**
         * The item’s degree of transparency
         * @return transparency percentage from 0 to 100
         */
        unsigned char getTransparency () const {  return this->transparency ;  }

        void setTransparency ( unsigned char newTransparency ) {  this->transparency = newTransparency ;  }

        /**
         * When the carrier moves, the item above it (this item) also moves
         */
        const std::string & getCarrier () const {  return this->carrier ;  }

        void setCarrier ( const std::string & itemBelow ) {  this->carrier = itemBelow ;  }

protected :

        void addFrame ( Picture *const frame )
        {
                this->frames.push_back( PicturePtr( frame ) ) ;
        }

        void addFrameOfShadow ( Picture *const frame )
        {
                this->shadows.push_back( PicturePtr( frame ) ) ;
        }

        virtual unsigned int firstFrame () const {  return 0 ;  }

        unsigned int getCurrentFrame () const {  return this->currentFrame ;  }

        void clearFrames ()
        {
                this->frames.clear ();
                this->shadows.clear ();
        }

        virtual void updateImage () = 0 ;

        virtual void updateShadow () = 0 ;

private :

        std::string uniqueName ;

        // current frame for item
        unsigned int currentFrame ;

        // the pictures of item
        std::vector< PicturePtr > frames ;

        // the pictures of item’s shadow
        std::vector< PicturePtr > shadows ;

        // the behaviour of item
        autouniqueptr< Behavior > behavior ;

        // the unique name of item below this one
        // the item above the carrier moves along with the moving carrier
        std::string carrier ;

        // the degree of image’s transparency as a percentage from 0 to 100
        unsigned char transparency ;

} ;

typedef multiptr < AbstractItem > AbstractItemPtr ;

#endif
