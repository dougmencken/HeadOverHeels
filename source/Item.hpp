// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
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
#include <cassert>

#include <WrappersAllegro.hpp>

#include "Timer.hpp"
#include "Picture.hpp"
#include "PoolOfPictures.hpp"
#include "Mediated.hpp"
#include "Shady.hpp"


namespace iso
{

class DescriptionOfItem ;
class Behavior ;


/**
 * Interface that defines attributes and operations for item in room
 */

class Item : public Mediated, public Shady
{

public:

        Item( const DescriptionOfItem * description, int z, const std::string& way ) ;

        Item( const Item& item ) ;

        virtual ~Item( ) ;

        virtual std::string whichKindOfItem () const {  return "abstract item" ;  }

       /**
        * Used for sorting items in container, this variant is okay for grid items
        */
        virtual bool operator < ( const Item& item ) const
        {
                return ( getZ() < item.getZ() + static_cast< int >( item.getHeight() ) );
        }

        bool isBehind ( const Item& item ) const
        {
                return  ( getZ() < item.getZ() + static_cast< int >( item.getHeight() ) ) &&
                        ( getX() < item.getX() + static_cast< int >( item.getWidthX() ) ) &&
                        ( getY() - static_cast< int >( getWidthY() ) < item.getY() ) ;
        }

        bool isBehindAt ( const Item& item, int x, int y, int z ) const
        {
                return  ( getZ() < z + static_cast< int >( item.getHeight() ) ) &&
                        ( getX() < x + static_cast< int >( item.getWidthX() ) ) &&
                        ( getY() - static_cast< int >( getWidthY() ) < y ) ;
        }

        virtual void calculateOffset () = 0 ;

        virtual bool updateItem () ;

        void animate () ;

        /**
         * Used for metamorphosis into bubbles, such as when player teleports
         */
        void metamorphInto ( const std::string& labelOfItem, const std::string& initiatedBy ) ;

        void changeOrientation ( const std::string & way ) ;

        /**
         * Change current frame for item. Change of frames is usually done via change of orientation
         * or via looping in sequence of animation. However there’re some cases when it’s necessary
         * to change frames manually. As example, in behavior of trampoline one frame is for rest
         * and another is for fold
         */
        void changeFrame ( size_t newFrame ) ;

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
         * May this item advance to given offset
         * @return true if position is free or false if there’s a collision,
         *         in the latter case place colliding items into stack of collisions
         */
        virtual bool canAdvanceTo ( int x, int y, int z ) ;

        bool intersectsWith ( const Item & item ) const ;

        bool doGraphicsOverlap ( const Item & item ) const ;

        bool doGraphicsOverlapAt ( const Item & item, std::pair < int, int > offset ) const ;

        /**
         * @param behavior Name of item’s behavior
         */
        void setBehaviorOf ( const std::string & behavior ) ;

        Behavior * getBehavior () const {  return behavior ;  }

        /**
         * Set animation going from first to last frame, which is by default
         */
        void doForthMotion () ;

        /**
         * Set animation going from last to first frame, backwards
         */
        void doBackwardsMotion () ;

        static PoolOfPictures & getPoolOfPictures () {  return * poolOfPictures ;  }

        const std::string& getUniqueName () const {  return uniqueName ;  }

        void setUniqueName ( const std::string& name ) {  this->uniqueName = name ;  }

        /**
         * Gives original label of item, label from item’s data may change via metamorphosis
         */
        const std::string& getOriginalLabel () const {  return originalLabel ;  }

        const DescriptionOfItem * getDescriptionOfItem () const {  return descriptionOfItem ;  }

        const std::string& getLabel () const ;

        int getX () const {  return xYet ;  }

        int getY () const {  return yYet ;  }

        int getZ () const {  return zYet ;  }

        void setZ ( const int newZ ) {  zYet = newZ ;  }

        /**
         * Width of item on X in isometric units
         */
        unsigned int getWidthX () const ;

        /**
         * Width of item on Y in isometric units
         */
        unsigned int getWidthY () const ;

        /**
         * Height, or width on Z, of item in isometric units
         */
        unsigned int getHeight () const {  return height ;  }

        void setHeight ( int newHeight ) {  height = newHeight ;  }

        bool isMortal () const ;

        size_t firstFrameForOrientation ( const std::string & way ) const ;

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

        /**
         * Set current orientation of item
         */
        void setOrientation ( const std::string & way ) {  this->orientation = way ;  }

        std::string getOrientation () const {  return orientation ;  }

        size_t firstFrame () const {  return firstFrameForOrientation( orientation ) ;  }

        bool animationFinished () const ;

        bool atExtraFrame () const ;

        const Picture & getRawImage () const {  return * getMotionAt( currentFrame ) ;  }

        Picture & getRawImageToChangeIt () const {  return * getMotionAt( currentFrame ) ;  }

        bool hasShadow () const {  return ! shadows.empty() ;  }

        const Picture & getImageOfShadow () const {  return * getShadowAt( currentFrame ) ;  }

        Picture & getProcessedImage () const {  return * processedImage ;  }

        virtual void freshProcessedImage () ;

        size_t howManyMotions () const {  return motion.size () ;  }

        const PicturePtr getMotionAt( size_t at ) const
        {
                return ( at < motion.size () ) ? motion[ at ] : PicturePtr () ;
        }

        void addFrame( const Picture& frame )
        {
                motion.push_back( PicturePtr( new Picture( frame ) ) ) ;
        }

        size_t howManyShadows () const {  return shadows.size () ;  }

        const PicturePtr getShadowAt( size_t at ) const
        {
                return ( at < shadows.size () ) ? shadows[ at ] : PicturePtr () ;
        }

        void addFrameOfShadow( const Picture& frame )
        {
                shadows.push_back( PicturePtr( new Picture( frame ) ) ) ;
        }

        /**
         * Distance of processed image from room’s origin ( 0, 0, 0 )
         */
        std::pair < int, int > getOffset () const {  return offset ;  }

        void setOffset ( const std::pair < int, int >& offset ) {  this->offset = offset ;  }

        /**
         * Distance of processed image from room’s origin ( 0, 0, 0 ) on X
         */
        int getOffsetX () const {  return offset.first ;  }

        /**
         * Distance of processed image from room’s origin ( 0, 0, 0 ) on Y
         */
        int getOffsetY () const {  return offset.second ;  }

        /**
         * Reference item below which would move this item
         */
        std::string getAnchor () const {  return this->anchor ;  }

        void setAnchor ( const std::string & item ) {  this->anchor = item ;  }

        /**
         * Set item’s ability to detect collisions
         */
        void setCollisionDetector ( bool detectThem ) {  collisionDetector = detectThem ;  }

        bool isCollisionDetector () const {  return collisionDetector ;  }

private:

        static PoolOfPictures * poolOfPictures ;

        void readGraphicsOfItem () ;

        /**
         * Extract frames for this item from file
         */
        static void createFrames ( Item * item, const DescriptionOfItem & data ) ;

        /**
         * Extract frames for shadow of this item from file
         */
        static void createShadowFrames ( Item * item, const DescriptionOfItem & data ) ;

        const DescriptionOfItem * descriptionOfItem ;

        std::string uniqueName ;

        std::string originalLabel ;

        /**
         * Image of this item with shadows from other items, for free item it is also masked
         */
        PicturePtr processedImage ;

        unsigned int height ;

        std::string orientation ;

        /**
         * Current frame for item
         */
        size_t currentFrame ;

        /**
         * True to reverse sequence of animation
         */
        bool backwardsMotion ;

        /**
         * Offset on ( X, Y ) from room’s point of origin
         */
        std::pair < int, int > offset ;

        /**
         * Whether this item detects collisions
         */
        bool collisionDetector ;

        /**
         * Pictures of item
         */
        std::vector< PicturePtr > motion ;

        /**
         * Pictures of item’s shadow
         */
        std::vector< PicturePtr > shadows ;

        /**
         * Timer for animation of item
         */
        autouniqueptr < Timer > motionTimer ;

        /**
         * Behavior of item
         */
        Behavior * behavior ;

        /**
         * Unique name of item below this one, when anchor moves item above it moves too
         */
        std::string anchor ;

protected:

        /**
         * Spacial position X in isometric units
         */
        int xYet ;

        /**
         * Spacial position Y in isometric units
         */
        int yYet ;

        /**
         * Spacial position Z in isometric units
         */
        int zYet ;

        virtual void updateImage () ;

        virtual void updateShadow () ;

};

typedef safeptr < Item > ItemPtr ;

}

#endif
