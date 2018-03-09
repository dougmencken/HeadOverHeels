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


namespace isomot
{

enum Coordinate
{
        CoordinateX = 1,
        CoordinateY = 2,
        CoordinateZ = 4,
        CoordinatesXYZ = CoordinateX | CoordinateY | CoordinateZ
} ;

enum ChangeOrAdd
{
        Change = 0,
        Add = 1
} ;

class ItemData ;
class Behavior ;


/**
 * Interface that defines attributes and operations for item in room
 */

class Item : public Mediated
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
         * Cambia la presentación gráfica del elemento, destruyendo la imagen procesada y señalando qué elementos
         * hay que volver a enmascarar
         * @param image Un fotograma del elemento
         */
        virtual void changeImage ( BITMAP* image ) = 0 ;

        /**
         * Cambia la sombra de la presentación gráfica del elemento, destruyendo la imagen procesada y señalando
         * qué elementos hay que volver a sombrear
         * @param image Una sombra de un fotograma del elemento
         */
        virtual void changeShadow ( BITMAP* shadow ) = 0 ;

        /**
         * Cambia el valor de la coordenada Z
         * @param value Valor que se sumará a la coordenada Z actual
         * @return true si se pudo cambiar el dato o false si hubo colisión y no hubo cambio
         */
        virtual bool addToZ ( int value ) = 0 ;

        /**
         * May this item move to given position
         * @param x X coordinate
         * @param y Y coordinate
         * @param z Z coordinate
         * @param what How to handle coordinates, just to change them or to add to existing values
         * @return true if position is free or false if there’s a collision,
         *         in the latter case place colliding items into stack of collisions
         */
        virtual bool checkPosition ( int x, int y, int z, const ChangeOrAdd& what ) ;

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

protected:

        std::string label ;

        std::string uniqueName ;

        ItemData * dataOfItem ;

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

        WhichShade myShady ;

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
         * Offset on ( X, Y ) from room’s point of origin
         */
        std::pair < int, int > offset ;

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

private:

        /**
         * Current frame for item
         */
        unsigned int frameIndex ;

        /**
         * True to reverse sequence of animation
         */
        bool backwardMotion ;

public:

        std::string getUniqueName () const {  return uniqueName ;  }

        void setUniqueName ( const std::string& name ) {  this->uniqueName = name ;  }

        ItemData * getDataOfItem () const {  return dataOfItem ;  }

        /**
         * Unique label of item
         */
        std::string getLabel () const {  return label ;  }

        /**
         * Posición espacial X en unidades isométricas
         */
        int getX () const {  return x ;  }

        /**
         * Posición espacial Y en unidades isométricas
         */
        int getY () const {  return y ;  }

        /**
         * Establece la posición espacial Z en unidades isométricas
         */
        void setZ ( const int z ) {  this->z = z ;  }

        /**
         * Posición espacial Z en unidades isométricas
         */
        int getZ () const {  return z ;  }

        /**
         * Anchura del elemento en unidades isométricas en el eje X
         */
        unsigned int getWidthX () const ;

        /**
         * Anchura del elemento en unidades isométricas en el eje Y
         */
        unsigned int getWidthY () const ;

        /**
         * Altura del elemento en unidades isométricas, anchura en el eje Z
         */
        unsigned int getHeight () const ;

        void setHeight ( int height ) ;

        /**
         * El elemento quitará una vida al jugador si éste lo toca
         * @return true si mata al jugador o false si es inofensivo
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
         * Tiempo en segundos necesario para que el elemento se mueva
         * @return Un número positivo
         */
        double getSpeed () const ;

        /**
        * Tiempo en segundos necesario para que el elemento caiga
        */
        double getWeight () const ;

        /**
         * Tiempo en segundos que será mostrado cada fotograma de la secuencia de animación
         */
        double getDelayBetweenFrames () const ;

        /**
         * Número total de fotogramas de la secuencia de animación
         */
        unsigned int countFrames () const ;

        /**
         * Set current orientation of item
         */
        void setOrientation ( const Way& way ) {  this->orientation = way ;  }

        Way getOrientation () const {  return orientation ;  }

        BITMAP * getRawImage () {  return rawImage ;  }

        BITMAP * getImageOfShadow () {  return shadow ;  }

        BITMAP * getProcessedImage () const {  return processedImage ;  }

        void setProcessedImage ( BITMAP * newImage ) ;

        void setWhichShade ( const WhichShade& shady ) {  this->myShady = shady ;  }

        WhichShade whichShade () const {  return this->myShady ;  }

        /**
         * Establece el desplazamiento del fotograma procesado desde el píxel que marca el punto origen
         * de la sala ( 0, 0, 0 )
         * @return Un par de valores, siendo el primero el desplazamiento en el eje X y el segundo en el eje Y
         */
        void setOffset ( const std::pair < int, int >& offset ) {  this->offset = offset ;  }

        /**
         * Desplazamiento del fotograma procesado desde el píxel que marca el punto origen de la sala ( 0, 0, 0 )
         * @return Un par de valores, siendo el primero el desplazamiento en el eje X y el segundo en el eje Y
         */
        std::pair < int, int > getOffset () const {  return offset ;  }

        /**
         * Desplazamiento del fotograma procesado en el eje X, desde el píxel que marca el punto origen
         * de la sala ( 0, 0, 0 )
         * @return Un número entero
         */
        int getOffsetX () const {  return offset.first ;  }

        /**
         * Desplazamiento del fotograma procesado en el eje X, desde el píxel que marca el punto origen
         * de la sala ( 0, 0, 0 )
         * @return Un número entero
         */
        int getOffsetY () const {  return offset.second ;  }

        Behavior * getBehavior () const {  return behavior ;  }

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
