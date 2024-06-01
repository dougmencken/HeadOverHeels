// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Item_hpp_
#define Item_hpp_

#include <string>
#include <utility>

#include <WrappersAllegro.hpp>

#include "Mediated.hpp"
#include "Shady.hpp"

#include "Timer.hpp"
#include "Picture.hpp"

namespace behaviors {  class Behavior ;  }
using behaviors::Behavior ;

class DescriptionOfItem ;


/**
 * Defines attributes and operations for an item in a room
 */

class Item : public Mediated, public Shady
{

public:

        Item( const DescriptionOfItem & description, int z, const std::string & where = "" ) ;

        Item( const Item& item ) ;

        virtual ~Item( ) ;

        virtual std::string whichItemClass () const {  return "abstract item" ;  }

        bool isBehind ( const Item & item ) const
        {
                return  ( getZ() < item.getZ() + item.getHeight() ) &&
                        ( getX() < item.getX() + item.getWidthX() ) &&
                        ( getY() - getWidthY() < item.getY() ) ;
        }

        bool isBehindAt ( const Item & item, int x, int y, int z ) const
        {
                return  ( getZ() < z + item.getHeight() ) &&
                        ( getX() < x + item.getWidthX() ) &&
                        ( getY() - getWidthY() < y ) ;
        }

        /**
         * For an item with behavior, update that behavior programmatically
         * @return true if the item can be updated thereafter (it didn’t disappear from the room)
         */
        virtual bool updateItem () ;

        void animate () ;

        /**
         * Metamorph into another kind of item, such as into bubbles when a character teleports
         */
        virtual void metamorphInto ( const std::string & newKind, const std::string & initiatedBy ) ;

        /**
         * Changes the current frame. Frames usually change when the angular orientation changes
         * or when looping in a sequence of animation. However there’re some cases when frames
         * are changed manually. As example, in the behavior of a spring stool the one frame
         * is for rest and the other is for fold
         */
        void changeFrame ( unsigned int newFrame ) ;

        /**
         * Add value to coordinate X
         * @return true on change or false when there’s collision
         */
        virtual bool addToX ( int value ) {  return addToPosition( value, 0, 0 ) ;  }

        /**
         * Add value to coordinate Y
         * @return true on change or false when there’s collision
         */
        virtual bool addToY ( int value ) {  return addToPosition( 0, value, 0 ) ;  }

        /**
         * Add value to coordinate Z
         * @return true on change or false when there’s collision
         */
        virtual bool addToZ ( int value ) {  return addToPosition( 0, 0, value ) ;  }

        /**
         * Change position of item
         * @param x Value to add to coordinate X
         * @param y Value to add to coordinate Y
         * @param z Value to add to coordinate Z
         * @return true on change or false when there’s collision
         */
        virtual bool addToPosition ( int x, int y, int z ) = 0 ;

        /**
         * May this item advance to the given offset or not
         * @return true if that location is free, or false if there’s a collision,
         *         in the latter case the colliding items are collected
         */
        virtual bool canAdvanceTo ( int x, int y, int z ) ;

        bool overlapsWith ( const Item & item ) const ;

        bool doGraphicsOverlap ( const Item & item ) const ;

        bool doGraphicsOverlapAt ( const Item & item, int x, int y ) const ;

        void setBehaviorOf ( const std::string & nameOfBehavior ) ;

        const autouniqueptr< Behavior > & getBehavior () const {  return behavior ;  }

        /**
         * Set animation going from first to last frame, which is by default
         */
        void doForthMotion () ;

        /**
         * Set animation going from last to first frame, backwards
         */
        void doBackwardsMotion () ;

        const std::string & getUniqueName () const {  return this->uniqueName ;  }

        void setUniqueName ( const std::string& name ) {  this->uniqueName = name ;  }

        /**
         * Gives the original kind of item, while the current kind may change via metamorphosis
         */
        const std::string & getOriginalKind () const {  return this->originalKind ;  }

        bool isMetamorphed () const {  return getKind() != getOriginalKind() ;  }

        const DescriptionOfItem & getDescriptionOfItem () const {  return * this->descriptionOfItem ;  }

        const std::string & getKind () const ;

        int getX () const {  return xYet ;  }

        int getY () const {  return yYet ;  }

        int getZ () const {  return zYet ;  }

        void setX ( int newX ) {  this->xYet = newX ;  }

        void setY ( int newY ) {  this->yYet = newY ;  }

        void setZ ( int newZ ) {  this->zYet = newZ ;  }

        /**
         * The width of this item on X in free units
         */
        unsigned int getUnsignedWidthX () const ;
        int getWidthX () const {  return static_cast< int >( getUnsignedWidthX () );  }

        /**
         * The width of this item on Y in free units
         */
        unsigned int getUnsignedWidthY () const ;
        int getWidthY () const {  return static_cast< int >( getUnsignedWidthY () );  }

        /**
         * The height, or width on Z, of this item in free units
         */
        unsigned int getUnsignedHeight () const {  return this->height ;  }
        int getHeight () const {  return static_cast< int >( getUnsignedHeight () );  }

        void changeHeightTo ( int newHeight ) {  this->height = newHeight ;  }

        bool isMortal () const ;

        /**
         * Time in seconds to move item
         */
        float getSpeed () const ;

        /**
         * Time in seconds to fall item
         */
        float getWeight () const ;

        /**
         * Time in seconds between each frame of item’s animation
         */
        float getDelayBetweenFrames () const ;

        const std::string & getHeading () const {  return this->heading ;  }

        void changeHeading ( const std::string & where ) ;

        unsigned int firstFrameWhenHeading ( const std::string & where ) const ;

        unsigned int firstFrame () const {  return firstFrameWhenHeading( getHeading() ) ;  }

        bool animationFinished () const ;

        bool atExtraFrame () const ;

        const Picture & getRawImage () const {  return * getFrameAt( currentFrame ) ;  }

        Picture & getRawImageToChangeIt () const {  return * getFrameAt( currentFrame ) ;  }

        bool hasShadow () const {  return ! shadows.empty() ;  }

        const Picture & getImageOfShadow () const {  return * getShadowAt( currentFrame ) ;  }

        Picture & getProcessedImage () const {  return * processedImage ;  }

        virtual void freshProcessedImage () ;

        size_t howManyFrames () const {  return this->frames.size () ;  }

        const PicturePtr getFrameAt( size_t at ) const
        {
                return ( at < howManyFrames() ) ? this->frames[ at ] : PicturePtr() ;
        }

        size_t howManyShadows () const {  return this->shadows.size () ;  }

        const PicturePtr getShadowAt( size_t at ) const
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

        /**
         * false to track if the item is colliding with something, true to ignore
         */
        void setIgnoreCollisions ( bool ignore ) {  ignoreCollisions = ignore ;  }

        bool isIgnoringCollisions () const {  return ignoreCollisions ;  }
        bool isNotIgnoringCollisions () const {  return ! ignoreCollisions ;  }

private:

        void readGraphicsOfItem () ;

        void addFrame( const Picture& frame )
        {
                this->frames.push_back( PicturePtr( new Picture( frame ) ) ) ;
        }

        void addFrameOfShadow( const Picture& frame )
        {
                this->shadows.push_back( PicturePtr( new Picture( frame ) ) ) ;
        }

        /**
         * Extract frames for this item from the picture file
         */
        void createFrames () ;

        /**
         * Extract frames for shadow of this item from the picture file
         */
        void createShadowFrames () ;

        const DescriptionOfItem * descriptionOfItem ;

        std::string uniqueName ;

        /**
         * The original kind of item, the current kind may change via metamorphosis
         */
        std::string originalKind ;

        /**
         * Image of this item with shadows from other items, for free item it is also masked
         */
        autouniqueptr< Picture > processedImage ;

        // the position in 3-dimensional space of this item’s lower north-west point, in free units
        int xYet ;
        int yYet ;
        int zYet ;

        // the height, or width along the z, of this item in free units
        unsigned int height ;

        // the angular orientation
        std::string heading ;

        // current frame for item
        size_t currentFrame ;

        // true to reverse the animation sequence
        bool backwardsMotion ;

        // whether to ignore that this item collides with something
        bool ignoreCollisions ;

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

        autouniqueptr < Timer > motionTimer ;

protected:

        virtual void updateImage () ;

        virtual void updateShadow () ;

};

typedef multiptr < Item > ItemPtr ;

#endif
