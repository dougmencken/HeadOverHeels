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
#include <allegro.h>

#include "Ism.hpp"
#include "Way.hpp"
#include "Timer.hpp"
#include "Mediated.hpp"
#include "Shady.hpp"


namespace isomot
{

class ItemData ;
class Behavior ;


/**
 * Interface that defines attributes and operations for item in room
 */

class Item : public Mediated, public Shady
{

public:

        Item( ItemData* data, int z, const Way& way ) ;

       /**
        * Constructor copia. No copia los atributos que son punteros
        */
        Item( const Item& item ) ;

        virtual ~Item( ) ;

        virtual std::string whichKindOfItem () const {  return "abstract item" ;  }

        /**
         * Update item’s behavior
         */
        virtual bool update () ;

        /**
         * Anima el elemento obteniendo el fotograma adecuado de la secuencia y cambiando su gráfico y su sombra
         * @return true si ha completado un ciclo de animación o false en caso contrario
         */
        bool animate () ;

        /**
         * Change data of item preserving its label, used when there’s metamorphosis such as when player teleports
         */
        void changeItemData ( ItemData * itemData, const std::string& initiatedBy ) ;

        void changeOrientation ( const Way& way ) ;

        /**
         * Change current frame for item. Change of frames is usually done via change of orientation
         * or via looping in sequence of animation. However there’re some cases when it’s necessary
         * to change frames manually, in situations not linked to animation or change of orientation.
         * As example, in behavior of trampoline one frame is for rest and another is for fold
         * @param frameIndex index of new frame
         */
        void changeFrame ( const unsigned int frameIndex ) ;

        /**
         * Change graphics of item
         */
        virtual void changeImage ( BITMAP* image ) = 0 ;

        /**
         * Change graphics of item’s shadow
         */
        virtual void changeShadow ( BITMAP* shadow ) ;

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

        /**
         * @param behavior Name of item’s behavior
         * @param extra Additional data to configure behavior
         */
        void assignBehavior( const std::string& behavior, void* extraData ) ;

        /**
         * Set animation going from first to last frame, which is by default
         */
        void setForthMotion () ;

        /**
         * Set animation going from last to first frame, backwards
         */
        void setReverseMotion () ;

private:

        std::string uniqueName ;

        std::string originalLabel ;

        /**
         * Current frame for item
         */
        unsigned int frameIndex ;

        /**
         * True to reverse sequence of animation
         */
        bool backwardMotion ;

        ItemData * dataOfItem ;

protected:

        /**
         * Spacial position X in isometric units
         */
        int x ;

        /**
         * Spacial position Y in isometric units
         */
        int y ;

        /**
         * Spacial position Z in isometric units
         */
        int z ;

        Way orientation ;

        /**
         * Offset on ( X, Y ) from room’s point of origin
         */
        std::pair < int, int > offset ;

        /**
         * Image of item, unprocessed, just read from file
         */
        BITMAP * rawImage ;

        /**
         * Image of item’s shadow
         */
        BITMAP * shadow ;

        /**
         * Image of this item with shadows from other items, for free item it is also masked
         */
        BITMAP * processedImage ;

        /**
         * Timer for animation of item
         */
        Timer * motionTimer ;

        /**
         * Behavior of item
         */
        Behavior * behavior ;

        /**
         * Reference item used to know if it would move when others are below
         */
        Item * anchor ;

public:

        const std::string& getUniqueName () const {  return uniqueName ;  }

        void setUniqueName ( const std::string& name ) {  this->uniqueName = name ;  }

        /**
         * Gives original label of item, because label from item’s data may change via metamorphosis
         */
        const std::string& getOriginalLabel () const {  return originalLabel ;  }

        ItemData * getDataOfItem () const {  return dataOfItem ;  }

        const std::string& getLabel () const ;

        int getX () const {  return x ;  }

        int getY () const {  return y ;  }

        int getZ () const {  return z ;  }

        void setZ ( const int z ) {  this->z = z ;  }

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
        unsigned int getHeight () const ;

        void setHeight ( int height ) ;

        /**
         * If true, item takes one life of character on touch
         */
        bool isMortal () const ;

        /**
         * How many different frames has item for four orientations ~
         * south, west, north, and east
         * @return 1 when there’s only one orientation,
         *         2 for frames of south and west, or
         *         4 for different frames of all of them
         */
        unsigned short howManyFramesForOrientations () const ;

        /**
         * Time in seconds to move item
         */
        double getSpeed () const ;

        /**
         * Time in seconds to fall item
         */
        double getWeight () const ;

        /**
         * Time in seconds between each frame of item’s animation
         */
        double getDelayBetweenFrames () const ;

        /**
         * How many frames has item’s animation sequence
         */
        unsigned int countFrames () const ;

        /**
         * Set current orientation of item
         */
        void setOrientation ( const Way& way ) {  this->orientation = way ;  }

        Way getOrientation () const {  return orientation ;  }

        BITMAP * getRawImage () const {  return rawImage ;  }

        BITMAP * getImageOfShadow () const {  return shadow ;  }

        BITMAP * getProcessedImage () const {  return processedImage ;  }

        void setProcessedImage ( BITMAP * newImage ) ;

        void binProcessedImage() {  if ( processedImage != nilPointer ) setProcessedImage( nilPointer ) ;  }

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

        Behavior * getBehavior () const {  return behavior ;  }

        void setBehavior ( Behavior * newBehavior ) {  behavior = newBehavior ;  }

        /**
         * Reference item used to know if it would move when others are below
         * @return such item or nil if there’s none
         */
        Item * getAnchor () const {  return this->anchor ;  }

        void setAnchor ( Item * item ) {  this->anchor = item ;  }

        unsigned int getIndexOfFrame() {  return this->frameIndex ;  }

};

}

#endif
