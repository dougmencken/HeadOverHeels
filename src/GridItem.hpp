// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef GridItem_hpp_
#define GridItem_hpp_

#include <utility>
#include <cmath>
#include <allegro.h>
#include "Ism.hpp"
#include "Item.hpp"
#include "Drawable.hpp"


namespace isomot
{

// Declaraciones adelantadas
class ItemData;

/**
 * Los elementos rejilla son aquellos que están colocados en una posición concreta de la
 * rejilla, y tienen las mismas anchuras que sus celdas. Los elementos rejilla son, básicamente,
 * los bloques estáticos del juego sobre los que el jugador va a saltar
 * No se puede cambiar las coordenadas X e Y de un elemento rejilla, solo se puede cambiar su
 * coordenada Z
 */
class GridItem : public Item, public Drawable
{

public:

        /**
         * Constructor
         * @param itemData Datos invariables del elemento
         * @param cx Celda que ocupa el elemento en el eje X
         * @param cy Celda que ocupa el elemento en el eje Y
         * @param z Posición espacial Z o a qué distancia está el elemento del suelo
         * @param direction Dirección inicial del elemento
         */
        GridItem( ItemData* itemData, int cx, int cy, int z, const Direction& direction ) ;

        virtual ~GridItem( ) ;

        /**
         * Draw this grid item
         */
        void draw ( BITMAP* where ) ;

        /**
         * Change graphics of this item, replacing the current image, and pointing out what to remask
         * @param image Un fotograma del elemento
         */
        void changeImage ( BITMAP* newImage ) ;

        /**
         * Change shadow for this item, replacing the current one, and figuring out what to reshadow
         * @param image Una sombra de un fotograma del elemento
         */
        void changeShadow ( BITMAP* newShadow ) ;

        /**
         * Solicita el sombreado del elemento
         * @param column Columna de elementos rejilla a la que pertenece este elemento
         */
        void requestCastShadow ( int column ) ;

        /**
         * Sombrea la imagen del elemento con la sombra de otro elemento
         * @param x Coordenada X de pantalla donde está situada la base del elemento que sombrea
         * @param y Coordenada Y de pantalla donde está situada la base del elemento que sombrea
         * @param shadow La sombra que se proyecta sobre el elemento
         * @param shadingScale Grado de opacidad de las sombras desde 0, sin sombras, hasta 256,
         *                     sombras totalmente opacas
         * @param transparency Grado de transparencia del elemento que sombrea
         */
        void castShadowImage ( int x, int y, BITMAP* shadow, short shadingScale, unsigned char transparency = 0 ) ;

        /**
         * Cambia el valor de la coordenada Z
         * @param value Valor que se sumará a la coordenada Z actual
         * @return true si se pudo cambiar el dato o false si hubo colisión y no hubo cambio
         */
        virtual bool addToZ ( int value ) ;

        /**
         * Cambia la altura del elemento
         * @param value Nueva altura del elemento
         * @return true si se pudo cambiar el dato o false si hubo colisión y no hubo cambio
         */
        virtual bool changeHeight ( int value ) ;

        /**
         * Cambia la altura del elemento
         * @param value Valor que se sumará a la altura actual
         * @return true si se pudo cambiar el dato o false si hubo colisión y no hubo cambio
         */
        virtual bool addHeight ( int value ) ;

protected:

        /**
         * Cambia el dato solicitado del elemento
         * @param value El nuevo valor
         * @param datum El dato que se quiere modificar: CoordinateZ, la coordenada Z, o Height, la altura
         * @param what Modo para interpretar el nuevo valor. Puede usarse Change para cambiarlo o Add para sumarlo
         * @return true si se pudo cambiar el dato o false si hubo colisión y no hubo cambio
         */
        virtual bool changeData ( int value, const Datum& datum, const WhatToDo& what ) ;

protected:

        /**
         * Posición en el eje X e Y, respectivamente, de la celda que ocupa el elemento en la sala
         */
        std::pair < int, int > cell ;

        /**
         * Índice de la columna de elementos a los que pertenece el elemento rejilla
         */
        int column ;

public:

        /**
         * Posición en el eje X de la celda que ocupa el elemento en la sala
         * @return Un número positivo
         */
        int getCellX () const {  return cell.first ;  }

        /**
         * Posición en el eje Y de la celda que ocupa el elemento en la sala
         * @return Un número positivo
         */
        int getCellY () const {  return cell.second ;  }

        /**
         * Establece el índice de la columna de elementos a los que pertenece el elemento rejilla
         * @param Un número positivo
         */
        void setColumn (const int column) {  this->column = column ;  }

        /**
         * Índice de la columna de elementos a los que pertenece el elemento rejilla
         * @return Un número positivo
         */
        int getColumn () const {  return column ;  }

};

}

#endif
