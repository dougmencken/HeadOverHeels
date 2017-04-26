// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

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

        virtual ~Menu( );

        /**
         * Dibuja el manú
         * @param where Imagen donde será dibujado
         */
        void draw ( BITMAP* where ) ;

        void redraw () ;

        /**
         * Responde a la pulsación de una tecla
         */
        void handleKey ( int key ) ;

        /**
         * Add option to menu
         */
        void addOption ( Label* label ) ;

        Label* getActiveOption () const {  return this->activeOption ;  }

        void setActiveOption ( Label* option ) ;

        /**
         * Make the first option active
         */
        void resetActiveOption () ;

        std::list < Label * > getEveryOption () {  return this->options ;  }

private:

        Label* handlerOfKeys ;

protected:

        /**
         * Selecciona la opción anterior del menú respecto de la actual
         */
        void previousOption () ;

        /**
         * Selecciona la opción siguiente del menú respecto de la actual
         */
        void nextOption () ;

protected:

        /**
         * Opciones de las que se compone el menú. El orden en la lista es el orden de aparición en el menú
         */
        std::list < Label * > options ;

        /**
         * La opción seleccionada
         */
        Label* activeOption ;

        BITMAP * whereToDraw ;

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
