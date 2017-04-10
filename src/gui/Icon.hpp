// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Icon_hpp_
#define Icon_hpp_

#include <allegro.h>
#include "Widget.hpp"


namespace gui
{

/**
 * Una imagen estática presente en la pantalla
 */
class Icon : public Widget
{

public:

       /**
        * Constructor
        * @param x Coordenada X de pantalla donde situar el elemento
        * @param x Coordenada X de pantalla donde situar el elemento
        * @param image La imagen a presentar
        */
        Icon( int x, int y, BITMAP * image ) ;

        virtual ~Icon( ) ;

       /**
        * Dibuja el elemento
        * @param where Imagen donde será dibujado
        */
        void draw ( BITMAP* where ) ;

        void handleKey ( int rawKey ) { /* do nothing */ }

private:

       /**
        * La imagen del icono
        */
        BITMAP* image ;

};

}

#endif
