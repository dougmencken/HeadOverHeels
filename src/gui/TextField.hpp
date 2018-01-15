// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef TextField_hpp_
#define TextField_hpp_

#include <string>
#include <list>
#include <allegro.h>
#include "Widget.hpp"


namespace gui
{

class Label ;

enum Alignment
{
        LeftAlignment,
        CenterAlignment,
        RightAlignment
};

/**
 * Stores text in multiple lines aligned left, center or right
 */

class TextField : public Widget
{

public:

        /**
        * Constructor
        * @param x Coordenada X de pantalla donde situar el elemento
        * @param x Coordenada X de pantalla donde situar el elemento
        * @param width Anchura del campo de texto
        * @param height Altura del campo de texto
        * @param alignment Alineación horizontal del texto: izquierda, centro o derecha
        */
        TextField( int x, int y, unsigned int width, unsigned int height, const Alignment& alignment ) ;

        virtual ~TextField( ) ;

        virtual void draw ( BITMAP * where ) ;

        void handleKey ( int rawKey ) { /* do nothing */ }

       /**
        * Añade una nueva línea al campo de texto
        */
        void addLine ( const std::string& text, const std::string& font, const std::string& color ) ;

       /**
        * Change position of text field
        */
        void moveTo ( int x, int y ) ;

        unsigned int getHeightOfField() {  return heightOfField ;  }

        Alignment getAlignment () {  return alignment ;  }

        void setAlignment( const Alignment& newAlignment ) ;

private:

       /**
        * Anchura del campo de texto
        */
        unsigned int width ;

       /**
        * Altura del campo de texto
        */
        unsigned int height ;

       /**
        * Alineación horizontal del texto: izquierda, centro o derecha
        */
        Alignment alignment ;

        unsigned int heightOfField ;

       /**
        * El campo de texto se compone de etiquetas
        */
        std::list < Label * > lines ;

};

}

#endif
