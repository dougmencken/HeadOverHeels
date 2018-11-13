// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
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

#include "Ism.hpp"
#include "Way.hpp"
#include "Timer.hpp"
#include "Picture.hpp"
#include "Mediated.hpp"
#include "Shady.hpp"


namespace iso
{

class ItemData ;
class Behavior ;


/**
 * Interface that defines attributes and operations for item in room
 */

class Item : public Mediated, public Shady
{

public:

        Item( const ItemData* data, int z, const Way& way ) ;

        Item( const Item& item ) ;

        virtual ~Item( ) ;

        virtual std::string whichKindOfItem () const {  return "abstract item" ;  }

       /**
        * Used for sorting items in container, this variant is okay for grid items
        */
        virtual bool operator< ( const Item& item ) const
        {
                return ( getZ() < item.getZ() + static_cast< int >( item.getHeight() ) );
        }

        virtual bool updateItem () ;

        /**
         * Anima el elemento obteniendo el fotograma adecuado de la secuencia y cambiando su gráfico y su sombra
         * @return true si ha completado un ciclo de animación o false en caso contrario
         */
        bool animate () ;

        /**
         * Used for metamorphosis into bubbles, such as when player teleports
         */
        void metamorphInto ( const std::string& labelOfItem, const std::string& initiatedBy ) ;

        void changeOrientation ( const Way& way ) ;

        /**
         * Change current frame for item. Change of frames is usually done via change of orientation
         * or via looping in sequence of animation. However there’re some cases when it’s necessary
         * to change frames manually, in situations not linked to animation or change of orientation.
         * As example, in behavior of trampoline one frame is for rest and another is for fold
         */
        void changeFrame ( const unsigned int newFrame ) ;

        /**
         * Change graphics of item
         */
        virtual void changeImage ( const Picture* image ) = 0 ;

        /**
         * Change graphics of item’s shadow
         */
        virtual void changeShadow ( const Picture* shadow ) ;

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

private:

        std::string uniqueName ;

        std::string originalLabel ;

        /**
         * Current frame for item
         */
        unsigned int currentFrame ;

        /**
         * True to reverse sequence of animation
         */
        bool backwardsMotion ;

        const ItemData * dataOfItem ;

        void readGraphicsOfItem () ;

        /**
         * Extract frames for this item from file
         */
        static void createFrames ( Item * item, const ItemData & data, const char * gfxPrefix ) ;

        /**
         * Extract frames for shadow of this item from file
         */
        static void createShadowFrames ( Item * item, const ItemData & data, const char * gfxPrefix ) ;

protected:

        /**
         * Spacial position X in isometric units
         */
        int xPos ;

        /**
         * Spacial position Y in isometric units
         */
        int yPos ;

        /**
         * Spacial position Z in isometric units
         */
        int zPos ;

        unsigned int height ;

        Way orientation ;

        /**
         * Offset on ( X, Y ) from room’s point of origin
         */
        std::pair < int, int > offset ;

        /**
         * Whether this item detects collisions
         */
        bool collisionDetector ;

        /**
         * Image of item, unprocessed, just read from file
         */
        const Picture * rawImage ;

        /**
         * Image of item’s shadow
         */
        const Picture * shadow ;

        /**
         * Image of this item with shadows from other items, for free item it is also masked
         */
        Picture * processedImage ;

        /**
         * Pictures of item
         */
        std::vector< const Picture * > motion ;

        /**
         * Pictures of item’s shadow
         */
        std::vector< const Picture * > shadows ;

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

public:

        const std::string& getUniqueName () const {  return uniqueName ;  }

        void setUniqueName ( const std::string& name ) {  this->uniqueName = name ;  }

        /**
         * Gives original label of item, label from item’s data may change via metamorphosis
         */
        const std::string& getOriginalLabel () const {  return originalLabel ;  }

        const ItemData * getDataOfItem () const {  return dataOfItem ;  }

        const std::string& getLabel () const ;

        int getX () const {  return xPos ;  }

        int getY () const {  return yPos ;  }

        int getZ () const {  return zPos ;  }

        void setZ ( const int newZ ) {  zPos = newZ ;  }

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

        /**
         * If true, item takes one life of character on touch
         */
        bool isMortal () const ;

        /**
         * @return 1 when there’s only one orientation,
         *         2 if there’re frames for south and west, or
         *         4 there’re frames for each orientation
         */
        unsigned short howManyOrientations () const ;

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
        void setOrientation ( const Way& way ) {  this->orientation = way ;  }

        Way getOrientation () const {  return orientation ;  }

        const Picture * getRawImage () const {  return rawImage ;  }

        const Picture * getImageOfShadow () const {  return shadow ;  }

        Picture * getProcessedImage () const {  return processedImage ;  }

        void setProcessedImage ( Picture * newImage ) ;

        void binProcessedImage() {  if ( processedImage != nilPointer ) setProcessedImage( nilPointer ) ;  }

        unsigned int howManyMotions () const {  return motion.size () ;  }

        const Picture * getMotionAt( size_t at ) const
        {
                assert( at < motion.size () );  /// return ( at < motion.size() ? motion[ at ] : nilPointer ) ;
                return ( rawImage != nilPointer ) ? motion[ at ] : nilPointer ;
        }

        void addFrame( const Picture& frame )
        {
                motion.push_back( new Picture( frame ) ) ;

                if ( rawImage == nilPointer && motion.size() == 1 )
                {
                        rawImage = motion.back() ;
                }
        }

        void clearMotionFrames ()
        {
                binProcessedImage ();

                rawImage = nilPointer ;

                for ( std::vector< const Picture * >::iterator it = motion.begin (); it != motion.end (); ++ it )
                        delete *it ;

                motion.clear ();
        }

        unsigned int howManyShadows () const {  return shadows.size () ;  }

        const Picture * getShadowAt( size_t at ) const
        {
                assert( at < shadows.size () ); /// return ( at < shadows.size() ? shadows[ at ] : nilPointer ) ;
                return ( shadow != nilPointer ) ? shadows[ at ] : nilPointer ;
        }

        void addFrameOfShadow( const Picture& frame )
        {
                shadows.push_back( new Picture( frame ) ) ;

                if ( shadow == nilPointer && shadows.size() == 1 )
                {
                        shadow = shadows.back() ;
                }
        }

        void clearShadowFrames ()
        {
                binProcessedImage ();

                shadow = nilPointer ;

                for ( std::vector< const Picture * >::iterator it = shadows.begin (); it != shadows.end (); ++ it )
                        delete *it ;

                shadows.clear() ;
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

        unsigned int getCurrentFrame() {  return this->currentFrame ;  }

        /**
         * Set item’s ability to detect collisions
         */
        void setCollisionDetector ( bool detectThem ) {  collisionDetector = detectThem ;  }

        bool isCollisionDetector () const {  return collisionDetector ;  }

};

typedef safeptr < Item > ItemPtr ;

}

#endif
