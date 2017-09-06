// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef MenuWithMultipleColumns_hpp_
#define MenuWithMultipleColumns_hpp_

#include "Menu.hpp"


namespace gui
{

class MenuWithMultipleColumns : public Menu
{

public:

        /**
         * Constructor de un menú de dos columnas
         * @param x Coordenada X de pantalla donde situar el menú
         * @param y Coordenada Y de pantalla donde situar el menú
         * @param secondColumnX Posición donde se situa la segunda columna del menú
         * @param rows Número de filas de la primera columna
         */
        MenuWithMultipleColumns( int x, int y, int secondColumnX, unsigned short rows );

        virtual ~MenuWithMultipleColumns( );

        void draw ( BITMAP* where ) ;

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

};

}

#endif
