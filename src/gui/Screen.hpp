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

#ifndef SCREEN_HPP_
#define SCREEN_HPP_

#include <cmath>
#include <list>
#include <algorithm>
#include <functional>
#include <allegro.h>
#include "Widget.hpp"

namespace gui
{

// Declaraciones adelantadas
class Action;

/**
 * La pantalla del juego: un contenedor donde mostrar los elementos de la interfaz
 * gráfica de usuario
 */
class Screen : public Widget
{

public:

        /**
         * Constructor
         * @param x Coordenada X de pantalla donde situar el elemento
         * @param y Coordenada Y de pantalla donde situar el elemento
         * @param destination Imagen donde se volcará la pantalla
         */
        Screen( unsigned int x, unsigned int y, BITMAP* destination ) ;

        virtual ~Screen( ) ;

        /**
         * Dibuja todo el contenido de la pantalla
         * @param destination Imagen donde será dibujada
         */
        void draw ( BITMAP* where ) ;

        /**
         * Responde a la pulsación de una tecla
         */
        void handleKey ( int key ) ;

        /**
         * Añade un nuevo componente a la pantalla
         * @param widget Un componente de la interfaz de usuario
         */
        void addWidget ( Widget* widget ) ;

private:

        /**
         * Color de fondo
         */
        int backgroundColor ;

        /**
         * Imagen de fondo de la pantalla
         */
        BITMAP* backgroundImage ;

        /**
         * Imagen donde se volcarán la pantalla
         */
        BITMAP* where ;

        /**
         * Elementos de la interfaz de usuario contenidos en la pantalla
         */
        std::list< Widget* > widgets ;

public: // Operaciones de consulta y actualización

        /**
         * Establece la imagen de fondo de la pantalla
         * @param image Una imagen
         */
        void setBackground( BITMAP* image ) {  this->backgroundImage = image ;  }

};

}

#endif
