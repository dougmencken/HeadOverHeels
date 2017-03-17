// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Screen_hpp_
#define Screen_hpp_

#include <cmath>
#include <list>
#include <algorithm>
#include <functional>
#include <allegro.h>
#include "Widget.hpp"

namespace gui
{

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
         * @param picture Imagen donde se volcará la pantalla
         */
        Screen( unsigned int x, unsigned int y, BITMAP* picture ) ;

        virtual ~Screen( ) ;

        /**
         * Dibuja todo el contenido de la pantalla
         * @param where Imagen donde será dibujada
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
        BITMAP * backgroundPicture ;

        /**
         * Imagen donde se volcarán la pantalla
         */
        BITMAP * where ;

        /**
         * Elementos de la interfaz de usuario contenidos en la pantalla
         */
        std::list< Widget* > widgets ;

        Action* escapeAction ;

public: // Operaciones de consulta y actualización

        /**
         * Establece la imagen de fondo de la pantalla
         * @param image Una imagen
         */
        void setBackground ( BITMAP* image ) {  backgroundPicture = image ;  }

        void setEscapeAction ( Action* action ) ;

};

}

#endif
