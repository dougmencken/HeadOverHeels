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

#ifndef Menu_hpp_
#define Menu_hpp_

#include <list>
#include <string>
#include <algorithm>
#include <functional>
#include <allegro.h>
#include "Widget.hpp"

namespace gui
{

// Declaraciones adelantadas
class Screen;
class Label;

/**
 * Un menú de la interfaz donde las opciones se eligen mediante las teclas Flecha Arriba
 * y Flecha Abajo y se seleccionan mediante la tecla Intro
 */
class Menu : public Widget
{

public:

        /**
         * Constructor
         * @param x Coordenada X de pantalla donde situar el menú
         * @param y Coordenada Y de pantalla donde situar el menú
         */
        Menu( unsigned int x, unsigned int y );

        /**
         * Constructor de un menú de dos columnas
         * @param x Coordenada X de pantalla donde situar el menú
         * @param y Coordenada Y de pantalla donde situar el menú
         * @param secondColumnX Posición donde se situa la segunda columna del menú
         * @param rows Número de filas de la primera columna
         */
        Menu( unsigned int x, unsigned int y, unsigned int secondColumnX, unsigned short rows );

        virtual ~Menu( );

        /**
         * Dibuja el manú
         * @param destination Imagen donde será dibujado
         */
        void draw ( BITMAP* destination ) ;

        /**
         * Responde a la pulsación de una tecla
         */
        void handleKey ( int key ) ;

        /**
         * Añade una opción al menú
         * @param action Una orden
         */
        void addOption ( Label* label ) ;

        /**
         * Añade una orden al menú estableciéndola como la acción seleccionada
         * @param action Una orden
         */
        void addActiveOption ( Label* label ) ;

        Label* getActiveOption () const {  return this->activeOption ;  }

        /**
         * Cambia una opción del menú
         * @param text Texto de la opción a sustituir
         * @param label Nueva opción
         */
        void changeOption ( const std::string& text, Label* label );

private:

        /**
         * Selecciona la opción anterior del menú respecto de la actual
         */
        void previousOption () ;

        /**
         * Selecciona la opción siguiente del menú respecto de la actual
         */
        void nextOption () ;

private:

        /**
         * Posición donde se situa la segunda columna del menú. Si vale 0 el menú tiene una columna
         */
        unsigned int secondColumnX ;

        /**
         * Número de filas de la primera columna. Cuando se sobrepase el número indicado se empezará
         * a situar el texto en la segunda columna. Si vale 0xffff el menú sólo tiene una columna
         */
        unsigned short rows ;


        /**
         * Opciones de las que se compone el menú. El orden en la lista es el orden de aparición en el menú
         */
        std::list< Label* > options ;

        /**
         * La opción seleccionada
         */
        Label* activeOption ;

        /**
         * Viñeta para cada opción de menú
         */
        BITMAP* optionImage ;

        /**
         * Viñeta para la opción seleccionada en el menú. Tamaño doble
         */
        BITMAP* selectedOptionImage ;

        /**
         * Viñeta para la opción seleccionada en el menú. Tamaño normal
         */
        BITMAP* selectedOptionImageMini ;

};

}

#endif
