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

        virtual void animate () = 0 ;

        virtual bool isAnimationFinished () const
        {
                if ( isAnimatedBackwards () )
                        return getCurrentFrame() == firstFrame() ;
                else
                        return getCurrentFrame() == lastFrame() ;
        }

        /**
         * Changes the current frame. Frames usually change when the angular orientation changes
         * or when looping in a sequence of animation. However there’re some cases when frames
         * are changed manually. As example, in the behavior of a spring stool one frame is for
         * resting and the other is for being fold
         */
        void changeFrame ( unsigned int newFrame ) ;

        void addFrameTo ( const std::string & sequence, Picture *const frame )
        {
                this->frames[ sequence ].push_back( PicturePtr( frame ) ) ;

                if ( getCurrentFrameSequence().empty() && ! sequence.empty() )
                        setCurrentFrameSequence( sequence );
        }

        void addFrameOfShadowTo ( const std::string & sequence, Picture *const frame )
        {
                this->shadows[ sequence ].push_back( PicturePtr( frame ) ) ;
        }

        bool hasShadow () const {  return ! shadows.empty() ;  }

        size_t howManyFramesIn ( const std::string & sequence ) const
        {
                for ( std::map< std::string, std::vector< PicturePtr > >::const_iterator it = this->frames.begin() ; it != this->frames.end() ; ++ it )
                        if ( it->first == sequence ) return it->second.size() ;

                return 0 ;
        }

        size_t howManyFramesInTheCurrentSequence () const {  return howManyFramesIn( getCurrentFrameSequence() ) ;  }

        size_t howManyFramesAtAll () const
        {
                size_t howManyFrames = 0 ;

                for ( std::map< std::string, std::vector< PicturePtr > >::const_iterator it = this->frames.begin() ; it != this->frames.end() ; ++ it )
                        howManyFrames += it->second.size() ;

                return howManyFrames ;
        }

        const Picture & getCurrentRawImageIn ( const std::string & sequence ) const {  return getNthFrameIn( sequence, getCurrentFrame() ) ;  }

        const Picture & getCurrentRawImage () const {  return getCurrentRawImageIn( getCurrentFrameSequence() ) ;  }

        Picture & getCurrentRawImageToChangeItIn ( const std::string & sequence ) const {  return getNthFrameIn( sequence, getCurrentFrame() ) ;  }

        Picture & getCurrentRawImageToChangeIt () const {  return getCurrentRawImageToChangeItIn( getCurrentFrameSequence() ) ;  }

        const Picture & getCurrentImageOfShadowIn ( const std::string & sequence ) const {  return getNthShadowIn( sequence, getCurrentFrame() ) ;  }

        const Picture & getCurrentImageOfShadow () const {  return getCurrentImageOfShadowIn( getCurrentFrameSequence() ) ;  }

        /**
         * Animate from the first to the last frame, which is by default
         */
        void doForthMotion ()
        {
                this->backwardsMotion = false ;
                changeFrame( firstFrame() );
        }

        /**
         * Animate from the last to the first frame, backwards
         */
        void doBackwardsMotion ()
        {
                this->backwardsMotion = true ;
                changeFrame( lastFrame() );
        }

        bool isAnimatedBackwards () const {  return this->backwardsMotion ;  }

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

        // in the current sequence
        virtual unsigned int firstFrame () const {  return 0 ;  }

        virtual unsigned int lastFrame () const
        {
                unsigned int howManyFrames = howManyFramesIn( getCurrentFrameSequence() );
                return ( howManyFrames > 0 ) ? howManyFrames - 1 : 0 ;
        }

        Picture & getNthFrameIn ( const std::string & sequence, unsigned int n ) const /* throws NoSuchPictureException */ ;

        Picture & getNthShadowIn ( const std::string & sequence, unsigned int n ) const /* throws NoSuchPictureException */ ;

        const std::string & getCurrentFrameSequence () const {  return this->currentSequence ;  }

        void setCurrentFrameSequence ( const std::string & sequence ) {  this->currentSequence = sequence ; setupAnimation() ;  }

        unsigned int getCurrentFrame () const {  return this->currentFrame ;  }

        void setupAnimation ()
        {
                changeFrame( isAnimatedBackwards() ? lastFrame() : firstFrame() );
        }

        void clearFrames ()
        {
                this->frames.clear ();
                this->shadows.clear ();
        }

        virtual void updateImage () = 0 ;

        virtual void updateShadow () = 0 ;

private :

        std::string uniqueName ;

        // the current sequence of frames
        std::string currentSequence ;

        // the current frame in the sequence
        unsigned int currentFrame ;

        // true to reverse the animation sequence
        bool backwardsMotion ;

        // the sequences of pictures of item
        std::map< std::string, std::vector< PicturePtr > > frames ;

        // the sequences of pictures of item’s shadow
        std::map< std::string, std::vector< PicturePtr > > shadows ;

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
