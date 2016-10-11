// Head over Heels (A remake by helmántika.org)
//
// © Copyright 2008 Jorge Rodríguez Santos <jorge@helmantika.org>
// © Copyright 1987 Ocean Software Ltd. (Original game)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef GRIDITEM_HPP_
#define GRIDITEM_HPP_

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
        GridItem( ItemData* itemData, int cx, int cy, int z, const Direction& direction );

        virtual ~GridItem();

        /**
         * Dibuja el elemento rejilla
         */
        void draw( BITMAP* where );

        /**
         * Cambia la presentación gráfica del elemento, destruyendo la imagen procesada y señalando qué elementos
         * hay que volver a enmascarar
         * @param image Un fotograma del elemento
         */
        void changeImage( BITMAP* image );

        /**
         * Cambia la sombra de la presentación gráfica del elemento, destruyendo la imagen procesada y señalando
         * qué elementos hay que volver a sombrear
         * @param image Una sombra de un fotograma del elemento
         */
        void changeShadow( BITMAP* shadow );

        /**
         * Solicita el sombreado del elemento
         * @param column Columna de elementos rejilla a la que pertenece este elemento
         */
        void requestCastShadow(int column);

        /**
         * Sombrea la imagen del elemento con la sombra de otro elemento
         * @param x Coordenada X de pantalla donde está situada la base del elemento que sombrea
         * @param y Coordenada Y de pantalla donde está situada la base del elemento que sombrea
         * @param shadow La sombra que se proyecta sobre el elemento
         * @param shadingScale Grado de opacidad de las sombras desde 0, sin sombras, hasta 256,
         *                     sombras totalmente opacas
         * @param transparency Grado de transparencia del elemento que sombrea
         */
        void castShadowImage(int x, int y, BITMAP* shadow, short shadingScale, unsigned char transparency = 0);

        /**
         * Cambia el valor de la coordenada Z
         * @param value Nuevo valor para Z
         * @return true si se pudo cambiar el dato o false si hubo colisión y no hubo cambio
         */
        virtual bool changeZ(int value);

        /**
         * Cambia el valor de la coordenada Z
         * @param value Valor que se sumará a la coordenada Z actual
         * @return true si se pudo cambiar el dato o false si hubo colisión y no hubo cambio
         */
        virtual bool addZ(int value);

        /**
         * Cambia la altura del elemento
         * @param value Nueva altura del elemento
         * @return true si se pudo cambiar el dato o false si hubo colisión y no hubo cambio
         */
        virtual bool changeHeight(int value);

        /**
         * Cambia la altura del elemento
         * @param value Valor que se sumará a la altura actual
         * @return true si se pudo cambiar el dato o false si hubo colisión y no hubo cambio
         */
        virtual bool addHeight(int value);

protected:

  /**
   * Cambia el dato solicitado del elemento
   * @param value El nuevo valor
   * @param datum El dato que se quiere modificar: CoordinateZ, la coordenada Z, o Height, la altura
   * @param mode Modo para interpretar el nuevo valor. Puede usarse Change para cambiarlo o Add para sumarlo
   * @return true si se pudo cambiar el dato o false si hubo colisión y no hubo cambio
   */
  virtual bool changeData(int value, const Datum& datum, const Mode& mode);

protected:

  /**
   * Posición en el eje X e Y, respectivamente, de la celda que ocupa el elemento en la sala
   */
  std::pair<int, int> cell;

  /**
   * Índice de la columna de elementos a los que pertenece el elemento rejilla
   */
  int column;

public: // Operaciones de consulta y actualización

  /**
   * Posición en el eje X de la celda que ocupa el elemento en la sala
   * @return Un número positivo
   */
  int getCellX() const { return cell.first; }

  /**
   * DPosición en el eje Y de la celda que ocupa el elemento en la sala
   * @return Un número positivo
   */
  int getCellY() const { return cell.second; }

  /**
   * Establece el índice de la columna de elementos a los que pertenece el elemento rejilla
   * @param Un número positivo
   */
  void setColumn(const int column) { this->column = column; }

  /**
   * Índice de la columna de elementos a los que pertenece el elemento rejilla
   * @return Un número positivo
   */
  int getColumn() const { return column; }

};

}

#endif //GRIDITEM_HPP_
