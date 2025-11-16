// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
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

#include "util.hpp"

#include "Mediated.hpp"
#include "NamedPicture.hpp"

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

        const autouniqueptr< Behavior > & getBehavior () const {  return this->behavior ;  }

        virtual void animate () = 0 ;

        virtual bool isAnimationFinished () const
        {
                if ( isAnimatedBackwards () )
                        return getCurrentFrame() == firstFrame() ;
                else
                        return getCurrentFrame() == lastFrame() ;
        }

        /**
         * Changes the shown frame (in the current sequence). Frames usually change when looping in
         * the sequence of animation. However there’re some cases when frames are changed manually.
         * As example, in the behavior of a spring stool the one frame is for rest and the other is
         * for being fold
         */
        virtual void changeFrameInTheCurrentSequence ( unsigned int newFrame ) ;

        virtual void addFrameTo ( const std::string & sequence, NamedPicture *const frame )
        {
                if ( sequence.empty() ) return ; // don’t add to ""

                this->frames[ sequence ].push_back( NamedPicturePtr( frame ) ) ;

                if ( getCurrentFrameSequence().empty() /* && ! sequence.empty() */ )
                        setCurrentFrameSequence( sequence );
        }

        size_t howManyFramesIn ( const std::string & sequence ) const
        {
                for ( std::map< std::string, std::vector< NamedPicturePtr > >::const_iterator it = this->frames.begin() ; it != this->frames.end() ; ++ it )
                        if ( it->first == sequence ) return it->second.size() ;

                return 0 ;
        }

        size_t howManyFramesInTheCurrentSequence () const {  return howManyFramesIn( getCurrentFrameSequence() ) ;  }

        size_t howManyFramesAtAll () const
        {
                size_t howManyFrames = 0 ;

                for ( std::map< std::string, std::vector< NamedPicturePtr > >::const_iterator it = this->frames.begin() ; it != this->frames.end() ; ++ it )
                        howManyFrames += it->second.size() ;

                return howManyFrames ;
        }

        const NamedPicture & getCurrentRawImageIn ( const std::string & sequence ) const {  return getNthFrameIn( sequence, getCurrentFrame() ) ;  }

        const NamedPicture & getCurrentRawImage () const {  return getCurrentRawImageIn( getCurrentFrameSequence() ) ;  }

        NamedPicture & getCurrentRawImageToChangeItIn ( const std::string & sequence ) const {  return getNthFrameIn( sequence, getCurrentFrame() ) ;  }

        NamedPicture & getCurrentRawImageToChangeIt () const {  return getCurrentRawImageToChangeItIn( getCurrentFrameSequence() ) ;  }

        /**
         * Animate from the first to the last frame, which is by default
         */
        void doForwardsMotion ()
        {
                this->backwardsMotion = false ;
                changeFrameInTheCurrentSequence( firstFrame() );
        }

        /**
         * Animate from the last to the first frame, backwards
         */
        void doBackwardsMotion ()
        {
                this->backwardsMotion = true ;
                changeFrameInTheCurrentSequence( lastFrame() );
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

        void setSequenceAndFrame ( const std::string & sequence, unsigned int frame )
        {
                setCurrentFrameSequence( sequence );
                changeFrameInTheCurrentSequence( frame );
        }

protected :

        // in the current sequence
        virtual unsigned int firstFrame () const {  return 0 ;  }

        virtual unsigned int lastFrame () const
        {
                unsigned int howManyFrames = howManyFramesInTheCurrentSequence() ;
                return ( howManyFrames > 0 ) ? howManyFrames - 1 : 0 ;
        }

        NamedPicture & getNthFrameIn ( const std::string & sequence, unsigned int n ) const /* throws NoSuchPictureException */ ;

        const std::string & getCurrentFrameSequence () const {  return this->currentSequence ;  }

        /**
         * The sequence of frames usually changes when the heading, aka angular orientation, changes
         */
        void setCurrentFrameSequence ( const std::string & sequence ) {  this->currentSequence = sequence ; resetAnimation() ;  }

        unsigned int getCurrentFrame () const {  return this->currentFrame ;  }

        void resetAnimation ()
        {
                changeFrameInTheCurrentSequence( isAnimatedBackwards() ? lastFrame() : firstFrame() );
        }

        virtual void clearFrames ()
        {
                this->frames.clear ();
        }

        virtual void updateImage () = 0 ;

private :

        std::string uniqueName ;

        // the current sequence of frames
        std::string currentSequence ;

        // the current frame in the sequence
        unsigned int currentFrame ;

        // true to reverse the animation sequence
        bool backwardsMotion ;

        // the sequences of pictures of item
        std::map< std::string, std::vector< NamedPicturePtr > > frames ;

        // the behaviour of item
        autouniqueptr< Behavior > behavior ;

        // the degree of image’s transparency as a percentage from 0 to 100
        unsigned char transparency ;

} ;

typedef multiptr < AbstractItem > AbstractItemPtr ;

#endif
